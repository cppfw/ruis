/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "tiling_area.hpp"

#include "../label/gap.hpp"

using namespace ruis;

namespace {
const ruis::real minimal_tile_size_pp = 30;
const ruis::real dragger_size_pp = 5;
} // namespace

namespace ruis::internal {
class dragger : public ruis::gap
{
	bool grabbed = false;
	ruis::vector2 grab_point;

	tiling_area& owner;

	ruis::mouse_cursor_stack::iterator arrows_cursor_iter;

public:
	// TODO: use shared_ref?
	std::shared_ptr<ruis::widget> prev_widget;
	std::shared_ptr<ruis::widget> next_widget;

	dragger(
		utki::shared_ref<ruis::context> c, //
		tiling_area& owner,
		styled<color> color
	) :
		ruis::widget(std::move(c), {}, {}),
		// clang-format off
		ruis::gap(this->context,
			{
				.color_params{
					.color = std::move(color)
				}
			}
		),
		// clang-format on
		owner(owner)
	{}

	bool on_mouse_button(const ruis::mouse_button_event& e) override
	{
		if (e.button != ruis::mouse_button::left) {
			return false;
		}

		this->grabbed = e.is_down;
		this->grab_point = e.pos;

		if (!this->grabbed) {
			if (!this->is_hovered()) {
				this->context.get().cursor_stack.pop(this->arrows_cursor_iter);
			}
		}

		return true;
	}

	bool on_mouse_move(const ruis::mouse_move_event& e) override
	{
		if (!this->grabbed) {
			return false;
		}

		auto [long_index, trans_index] = this->owner.get_long_trans_indices();

		auto delta = (e.pos - this->grab_point)[long_index];

		// delta[trans_index] = ruis::real(0);

		ASSERT(this->prev_widget)
		ASSERT(this->next_widget)

		auto old_prev_dim = this->prev_widget->rect().d[long_index];
		auto old_next_dim = this->next_widget->rect().d[long_index];

		auto new_prev_dim = old_prev_dim + delta;
		auto new_next_dim = old_next_dim - delta;

		using std::max;
		using std::min;

		// clamp new tile dimensions to minimum tile size
		new_prev_dim = max(new_prev_dim, this->owner.min_tile_size);
		new_next_dim = max(new_next_dim, this->owner.min_tile_size);

		if (delta >= 0) {
			delta = min(delta, old_next_dim - new_next_dim);
		} else {
			delta = max(delta, new_prev_dim - old_prev_dim);
		}

		vec2 delta_vec;
		delta_vec[long_index] = delta;
		delta_vec[trans_index] = 0;

		this->move_by(delta_vec);

		this->prev_widget->resize_by(delta_vec);

		this->next_widget->resize_by(-delta_vec);
		this->next_widget->move_by(delta_vec);

		this->owner.notify_tiles_resized();

		return true;
	}

	void on_hovered_change(unsigned pointer_id) override
	{
		if (this->grabbed) {
			return;
		}

		if (this->is_hovered() || grabbed) {
			this->arrows_cursor_iter = this->context.get().cursor_stack.push( //
				[&]() {
					if (this->owner.is_vertical()) {
						return ruis::mouse_cursor::up_down_arrow;
					} else {
						return ruis::mouse_cursor::left_right_arrow;
					}
				}()
			);
		} else {
			this->context.get().cursor_stack.pop(this->arrows_cursor_iter);
		}
	}

	void render(const ruis::matrix4& matrix) const override
	{
		if (this->is_hovered() || this->grabbed) {
			this->ruis::gap::render(matrix);
		}
	}
};
} // namespace ruis::internal

tiling_area::tiling_area(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	ruis::widget_list children
) :
	ruis::widget(
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params) //
	),
	ruis::oriented({.vertical = false}),
	ruis::content_wrapping(ruis::make::container(
		this->context,
		{},
		std::move(children) //
	)),
	// clang-format off
	ruis::container(this->context,
		{},
		{
			this->content_container
		}
	),
	// clang-format on
	min_tile_size(this->context.get().units.pp_to_px(minimal_tile_size_pp)),
	dragger_size(this->context.get().units.pp_to_px(dragger_size_pp)),
	params([&]() {
		constexpr uint32_t default_dragger_color = 0xffff8080;

		if (params.tiling_area_params.dragger_color.get().is_undefined()) {
			params.tiling_area_params.dragger_color = default_dragger_color;
		}
		return std::move(params.tiling_area_params);
	}())
{
	this->content().move_to({0, 0});
}

void tiling_area::on_lay_out()
{
	auto [long_index, trans_index] = this->get_long_trans_indices();

	using std::max;

	// calculate current length of all tiles
	ruis::real tiles_length = 0;

	for (const auto& t : this->content()) {
		tiles_length += max( //
			t.get().rect().d[long_index],
			this->min_tile_size
		);
	}

	const auto& content_dims = this->rect().d;

	using std::round;

	// arrange tiles
	if (content_dims[long_index] >= tiles_length) {
		ruis::vector2 pos{0, 0};
		for (auto& t : this->content()) {
			ruis::real tile_length = max( //
				t.get().rect().d[long_index],
				this->min_tile_size
			);

			ASSERT(tiles_length > 0)

			ruis::vector2 dims;
			dims[trans_index] = content_dims[trans_index];
			dims[long_index] = content_dims[long_index] * (tile_length / tiles_length);
			dims = round(dims);
			t.get().resize(dims);
			t.get().move_to(pos);
			pos[long_index] += dims[long_index];
		}
	} else {
		ruis::real left_length = content_dims[long_index];

		ruis::vector2 pos{0, 0};

		for (auto& t : this->content()) {
			ruis::real tile_length = max(t.get().rect().d[long_index], this->min_tile_size);

			ASSERT(tiles_length > 0)

			ruis::vector2 dims;
			dims[trans_index] = content_dims[trans_index];
			dims[long_index] = left_length * (tile_length / tiles_length);
			if (dims[long_index] <= this->min_tile_size) {
				dims[long_index] = this->min_tile_size;
			}
			tiles_length -= tile_length;
			left_length -= dims[long_index];
			dims = round(dims);

			t.get().resize(dims);
			t.get().move_to(pos);
			pos[long_index] += dims[long_index];
		}
	}

	this->content().resize(content_dims);

	// ====================
	// = lay out draggers =

	ASSERT(this->size() >= 1)

	auto num_draggers = [&]() -> size_t {
		if (this->content().empty()) {
			return 0;
		} else {
			return this->content().size() - 1;
		}
	}();

	// remove redundant draggers
	while (this->size() - 1 > num_draggers) {
		this->pop_back();
	}

	// add missing draggers
	while (this->size() - 1 < num_draggers) {
		this->push_back(utki::make_shared<internal::dragger>(
			this->context, //
			*this,
			this->params.dragger_color
		));
	}

	ruis::vector2 dragger_dims;
	dragger_dims[long_index] = this->dragger_size;
	dragger_dims[trans_index] = this->rect().d[trans_index];

	for (auto i = std::next(this->begin()); i != this->end(); ++i) {
		auto index = size_t(std::distance(this->begin(), i)) - 1;

		ASSERT(index < this->content().size())

		auto& dragger = dynamic_cast<internal::dragger&>(i->get());

		dragger.prev_widget = this->content().children()[index].to_shared_ptr();
		dragger.next_widget = this->content().children()[index + 1].to_shared_ptr();

		dragger.resize(dragger_dims);

		ruis::vector2 dragger_pos;
		dragger_pos[trans_index] = ruis::real(0);
		dragger_pos[long_index] = round(dragger.next_widget->rect().p[long_index] - this->dragger_size / 2);
		dragger.move_to(dragger_pos);
	}

	this->notify_tiles_resized();
}

ruis::vector2 tiling_area::measure(const ruis::vector2& quotum) const
{
	auto [long_index, trans_index] = this->get_long_trans_indices();

	ruis::vector2 ret;

	// longitudinal index
	if (quotum[long_index] < 0) {
		ret[long_index] = this->min_tile_size * real(this->content().size());
	} else {
		ret[long_index] = quotum[long_index];
	}

	// transverse index
	if (quotum[trans_index] < 0) {
		vec2 tile_quotum;
		tile_quotum[long_index] = this->min_tile_size;
		tile_quotum[trans_index] = -1;

		real d = 0;
		for (const auto& w : this->content()) {
			auto measured = w.get().measure(tile_quotum);
			using std::max;
			d = max(d, measured[trans_index]);
		}
		ret[trans_index] = d;
	} else {
		ret[trans_index] = quotum[trans_index];
	}

	return ret;
}

void tiling_area::notify_tiles_resized()
{
	if (this->tiles_resized_handler) {
		this->tiles_resized_handler(*this);
	}
}

utki::shared_ref<ruis::tiling_area> make::tiling_area(
	utki::shared_ref<ruis::context> context, //
	ruis::tiling_area::all_parameters params,
	ruis::widget_list children
)
{
	if (params.tiling_area_params.dragger_color.get().is_undefined()) {
		params.tiling_area_params.dragger_color = context.get().style().get_color_cursor();
	}

	return utki::make_shared<ruis::tiling_area>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}
