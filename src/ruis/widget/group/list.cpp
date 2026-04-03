/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "list.hpp"

#include <utki/config.hpp>

#include "../../context.hpp"

using namespace ruis;

list::list(
	utki::shared_ref<ruis::context> context, //
	all_parameters params
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	ruis::container(
		this->context,
		{
			.container_params{
				.layout = layout::trivial
			}
		},
		{}
	),
	// clang-format on
	oriented(std::move(params.oriented_params)),
	list_widget(
		this->context, //
		std::move(params.list_params)
	)
{}

void list::on_lay_out()
{
	//	TRACE(<< "list::on_lay_out(): invoked" << std::endl)

	this->num_tail_items = 0; // 0 means that it needs to be recomputed

	this->update_children_list();

	// defer the scroll position change notification, because layouting happens during render phase
	this->context.get().post_to_ui_thread([wl = utki::make_weak_from(*this)]() {
		if (auto l = wl.lock()) {
			l->notify_scroll_pos_changed();
		}
	});
}

real list::calc_num_visible_items() const noexcept
{
	// calculate number of visible items,
	// this number can have fraction part because of partially visible items
	real items_num = 0;

	unsigned index = this->get_long_index();

	for (auto& c : this->children()) {
		auto dim = c.get().rect().d[index];
		auto visible_dim = dim;
		auto pos = c.get().rect().p[index];
		if (pos < 0) {
			visible_dim += pos;
		}
		auto list_dim = this->rect().d[index];
		auto end_pos = pos + dim;
		if (end_pos > list_dim) {
			visible_dim -= (end_pos - list_dim);
		}

		items_num += visible_dim / dim;
	}

	return items_num;
}

real list::get_scroll_band() const noexcept
{
	if (this->get_provider().count() == 0) {
		return 0;
	}

	auto items_num = this->calc_num_visible_items();

	return items_num / ruis::real(this->get_provider().count());
}

real list::get_scroll_factor() const noexcept
{
	if (this->get_provider().count() == 0) {
		return 0;
	}

	if (this->children().size() == 0) {
		return 0;
	}

	utki::assert(this->get_provider().count() >= this->num_tail_items, SL);

	size_t num_items_without_tail = this->get_provider().count() - this->num_tail_items;

	if (num_items_without_tail == 0) {
		return 0;
	}

	if (this->num_tail_items == 0) {
		return 0;
	}

	auto long_index = this->get_long_index();

	real list_dim = this->rect().d[long_index];

	// calculate average item dimension from visible items
	auto average_item_dim = list_dim / this->calc_num_visible_items();

	if (average_item_dim <= 0) {
		return 0;
	}

	return (real(real(this->pos_index) + this->pos_offset / this->children().front().get().rect().d[long_index]) *
			average_item_dim) /
		(real(real(num_items_without_tail) + this->first_tail_item_offset / this->first_tail_item_dim) *
		 average_item_dim);
}

void list::set_scroll_factor(
	real factor, //
	bool notify_scroll_change
)
{
	if (this->get_provider().count() == 0) {
		return;
	}

	if (this->num_tail_items == 0) {
		this->update_tail_items_info();
	}

	size_t old_index = this->pos_index;
	real old_offset = this->pos_offset;

	this->pos_index = size_t(factor * real(this->get_provider().count() - this->num_tail_items));

	//	TRACE(<< "list::setScrollPosAsFactor(): this->pos_index = " << this->pos_index << std::endl)

	using std::round;

	if (this->get_provider().count() != this->num_tail_items) {
		real int_factor = real(this->pos_index) / real(this->get_provider().count() - this->num_tail_items);

		if (this->children().size() != 0) {
			real d = this->rect().d[this->get_long_index()];
			d = (d + this->first_tail_item_offset) / real(this->num_tail_items);

			this->pos_offset = round(
				d * (factor - int_factor) * real(this->get_provider().count() - this->num_tail_items) +
				factor * this->first_tail_item_offset
			);
		} else {
			this->pos_offset = 0;
		}
	} else {
		ASSERT(this->pos_index == 0)
		this->pos_offset = round(factor * this->first_tail_item_offset);
	}

	this->update_children_list();

	if (notify_scroll_change) {
		this->notify_scroll_pos_changed(old_index, old_offset);
	}
}

// TODO: refactor
bool list::arrange_widget(
	const utki::shared_ref<widget>& w,
	real& pos,
	bool added,
	size_t index,
	widget_list::const_iterator& insert_before
)
{
	vec2 dim = dims_for_widget(w.get(), this->rect().d);

	w.get().resize(dim);

	unsigned long_index = this->get_long_index();
	unsigned trans_index = this->get_trans_index();

	{
		vec2 to;
		to[long_index] = pos;
		to[trans_index] = 0;
		w.get().move_to(to);
	}
	pos += w.get().rect().d[long_index];

	if (pos > 0) {
		if (!added) {
			insert_before = this->insert(w, insert_before);
		}
		++insert_before;
		if (this->added_index > index) {
			this->added_index = index;
		}
	} else {
		++this->pos_index;
		this->pos_offset -= w.get().rect().d[long_index];
		if (added) {
			insert_before = this->erase(insert_before);
			++this->added_index;
		}
	}

	if (w.get().rect().p[long_index] + w.get().rect().d[long_index] >= this->rect().d[long_index]) {
		return true;
	}

	return false;
}

// TODO: refactor
void list::update_children_list()
{
	if (this->num_tail_items == 0) {
		this->update_tail_items_info();
	}

	if (this->pos_index == this->first_tail_item_index) {
		if (this->pos_offset > this->first_tail_item_offset) {
			this->pos_offset = this->first_tail_item_offset;
		}
	} else if (this->pos_index > this->first_tail_item_index) {
		this->pos_index = this->first_tail_item_index;
		this->pos_offset = this->first_tail_item_offset;
	}

	real pos = -this->pos_offset;

	//	TRACE(<< "list::update_children_list(): this->added_index = " << this->added_index << " this->pos_index =
	//" << this->pos_index << std::endl)

	// remove widgets from top
	for (; this->children().size() != 0 && this->added_index < this->pos_index; ++this->added_index) {
		auto i = this->children().begin();
		this->erase(i);
	}

	auto iter = this->children().begin();
	size_t iter_index = this->added_index;
	const size_t iter_end_index = iter_index + this->children().size();
	size_t index = this->pos_index;
	for (bool is_last = false; index < this->get_provider().count() && !is_last;) {
		bool is_already_added = false;
		auto w = [&]() {
			if (iter_index <= index && index < iter_end_index && iter != this->children().end()) {
				++iter_index;
				is_already_added = true;
				return *iter;
			} else {
				is_already_added = false;
				return this->get_provider().get_widget(index);
			}
		}();

		is_last = this->arrange_widget(
			w, //
			pos,
			is_already_added,
			index,
			iter
		);
		++index;
	}

	// remove rest
	if (iter_index < iter_end_index) {
		utki::assert(iter != this->children().end(), SL);
		for (; iter != this->children().end(); ++iter_index) {
			iter = this->erase(iter);
		}
	}
}

void list::update_tail_items_info()
{
	this->num_tail_items = 0;

	if (this->get_provider().count() == 0) {
		return;
	}

	unsigned long_index = this->get_long_index();

	real dim = this->rect().d[long_index];

	ASSERT(this->get_provider().count() > 0)

	for (size_t i = this->get_provider().count(); i != 0 && dim > 0; --i) {
		++this->num_tail_items;

		auto w = this->get_provider().get_widget(i - 1);

		vec2 d = dims_for_widget(w.get(), this->rect().d);

		auto item_dim = d[long_index];
		dim -= item_dim;
		this->first_tail_item_dim = item_dim;
	}

	this->first_tail_item_index = this->get_provider().count() - this->num_tail_items;

	if (dim > 0) {
		this->first_tail_item_offset = -1;
	} else {
		this->first_tail_item_offset = -dim;
	}

	if (this->first_tail_item_dim <= 0) {
		this->first_tail_item_dim = 1;
	}
}

void list::notify_scroll_pos_changed()
{
	if (this->scroll_change_handler) {
		this->scroll_change_handler(*this);
	}
}

void list::notify_scroll_pos_changed(size_t old_index, real old_offset)
{
	if (old_index != this->pos_index || old_offset != this->pos_offset) {
		this->notify_scroll_pos_changed();
	}
}

real list::scroll_by(real delta)
{
	using std::round;
	delta = round(delta);

	size_t old_index = this->pos_index;
	real old_offset = this->pos_offset;

	// list's longitudinal direction index
	unsigned long_index = this->get_long_index();

	real scrolled_by = 0;

	std::cout << "delta = " << delta << std::endl;

	// TODO: added for now as delta of 0 causes unexpected scrolled_by valu returned.
	//       figure out if this can be handled as delta >= 0 condition.
	if (delta == 0) {
		return scrolled_by;
	}

	if (delta > 0) {
		std::cout << "delta > 0" << std::endl;

		// go through visible widgets first
		for (auto& c : this->children()) {
			// Since we are scrolling 'up', assume we are dealing with the first visible widget on each iteration,
			// because previous one which was first visible is aready scrolled away upwards.
			auto dim = c.get().rect().d[long_index] - this->pos_offset;
			if (delta <= dim) {
				// We can scroll the whole delta and the same widget remains the first visible one,
				// only its position offset changes.
				std::cout << "delta <= dim, delta = " << delta << ", dim = " << dim << std::endl;
				this->pos_offset += delta;
				scrolled_by += delta;
				delta = 0;
				break;
			}

			std::cout << "dim <= delta, delta = " << delta << ", dim = " << dim << std::endl;

			delta -= dim;
			scrolled_by += dim;
			this->pos_offset = 0;
			++this->pos_index;
		}

		// if there is still distance to scroll, then go through the rest of the widgets
		if (delta > 0) {
			std::cout << "delta > 0: delta = " << delta << std::endl;
			utki::assert(
				this->pos_index > this->added_index + this->children().size(),
				[&](auto& o) {
					o << "this->pos_index = " << this->pos_index << " this->added_index = " << this->added_index
					  << " this->children().size() = " << this->children().size();
				},
				SL
			);
			for (; this->pos_index < this->first_tail_item_index;) {
				auto w = this->get_provider().get_widget(this->pos_index);
				vec2 d = dims_for_widget(w.get(), this->rect().d);

				// this is just optimization, to avoid creating same widget twice
				this->push_back(std::move(w));

				if (d[long_index] > delta) {
					this->pos_offset = delta;
					break;
				}
				delta -= d[long_index];
				scrolled_by += d[long_index];
				utki::assert(this->pos_offset == 0, SL);
				++this->pos_index;
			}
		}
	} else {
		utki::assert(delta < 0, SL);
		delta = -delta;
		if (delta <= this->pos_offset) {
			std::cout << "delta <= this->pos_offset" << std::endl;
			this->pos_offset -= delta;
			scrolled_by -= delta;
		} else {
			utki::assert(this->added_index == this->pos_index, SL);
			delta -= this->pos_offset;
			scrolled_by -= this->pos_offset;
			this->pos_offset = 0;

			for (; this->pos_index > 0;) {
				utki::assert(this->added_index == this->pos_index, SL);
				--this->pos_index;
				utki::assert(
					[&]() -> bool {
						return this->pos_index < this->get_provider().count();
					},
					SL
				);
				auto w = this->get_provider().get_widget(this->pos_index);
				vec2 d = dims_for_widget(w.get(), this->rect().d);
				auto long_dim = d[long_index];

				// this is just optimization, to avoid creating same widget twice
				this->insert(
					std::move(w), //
					this->children().begin()
				);
				utki::assert(this->added_index > 0, SL);
				--this->added_index;

				if (delta <= long_dim) {
					std::cout << "delta <= long_dim: delta = " << delta << std::endl;
					this->pos_offset = long_dim - delta;
					scrolled_by -= delta;
					break;
				}
				std::cout << "delta > long_dim: delta = " << delta << std::endl;
				delta -= long_dim;
				scrolled_by -= long_dim;
			}
		}
	}

	// this will update the scrolling position to the bottom of the list if we have scrolled past the end of the list
	this->update_children_list();

	// TODO: detect the situation when we have scrolled past the list's end and update the scrolled_by value accordingly

	this->notify_scroll_pos_changed(old_index, old_offset);

	std::cout << "list: scrolled_by = " << scrolled_by << std::endl;

	return scrolled_by;
}

ruis::vec2 list::measure(const ruis::vec2& quotum) const
{
	unsigned long_index = this->get_long_index();
	unsigned trans_index = this->get_trans_index();

	vec2 ret(quotum);

	using std::max;
	ret[long_index] = max(ret[long_index], real(0)); // clamp bottom

	if (ret[trans_index] > 0) {
		return ret;
	}

	ret[trans_index] = 0;

	for (const auto& w : this->children()) {
		ret[trans_index] = max(ret[trans_index], w.get().rect().d[trans_index]); // clamp bottom
	}

	return ret;
}

void list::handle_model_change()
{
	this->num_tail_items = 0; // 0 means that it needs to be recomputed

	this->clear();
	this->added_index = size_t(-1);

	this->update_children_list();

	if (this->model_change_handler) {
		this->model_change_handler(*this);
	}
}
