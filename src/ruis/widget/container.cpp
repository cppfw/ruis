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

#include "container.hpp"

#include <utki/config.hpp>

#include "../context.hpp"
#include "../util/util.hpp"

using namespace ruis;

container::container(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	widget_list children
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	children_list(std::move(children)),
	layout([&]() {
		if (!params.container_params.layout) {
			return ruis::layout::trivial;
		}
		return utki::shared_ref(params.container_params.layout);
	}())
{
	for (auto& w : this->children()) {
		if (w.get().parent()) {
			throw std::invalid_argument(
				"container::container(): one of the supplied child widgets is already added to some other container"
			);
		}
		this->set_widget_parent(w.get());
	}
}

void container::render_child(
	const matrix4& matrix, //
	const widget& c
) const
{
	if (!c.is_visible()) {
		return;
	}

	ruis::matrix4 matr(matrix);
	matr.translate(c.rect().p);

	c.render_internal(matr);
}

void container::render(const ruis::matrix4& matrix) const
{
	for (auto& w : this->children()) {
		this->render_child(matrix, w.get());
	}
}

bool container::on_mouse_button(const mouse_button_event& e)
{
	//	TRACE(<< "container::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos <<
	// std::endl)

	blocked_flag_guard blocked_guard(this->is_blocked);

	// check if mouse captured
	{
		auto i = this->mouse_capture_map.find(e.pointer_id);
		if (i != this->mouse_capture_map.end()) {
			if (auto w = i->second.capturing_widget.lock()) {
				if (w->is_interactive()) {
					w->on_mouse_button(mouse_button_event{e.is_down, e.pos - w->rect().p, e.button, e.pointer_id});
					w->set_hovered(w->rect().overlaps(e.pos), e.pointer_id);

					unsigned& num_buttons_captured = i->second.num_buttons_captured;
					if (e.is_down) {
						// if we get button down event for mouse capturing widget, then it is not for the buttons
						// which are already down, so we increase the button counter.
						++num_buttons_captured;
					} else {
						// If we get button up event,
						//  then it is for one of the buttons which was down before, so we decrease the button counter.
						--num_buttons_captured;
					}
					if (num_buttons_captured == 0) {
						this->mouse_capture_map.erase(i);
					}

					// doesn't matter what to return because parent widget also captured
					// the mouse and in this case the return value is ignored
					return true;
				}
			}
			this->mouse_capture_map.erase(i);
		}
	}

	// call children in reverse order
	for (auto i = this->children().rbegin(); i != this->children().rend(); ++i) {
		auto& c = i->get();

		if (!c.is_interactive()) {
			continue;
		}

		if (!c.rect().overlaps(e.pos)) {
			continue;
		}

		// Sometimes mouse click event comes without prior mouse move,
		// but, since we get mouse click, then the widget was hovered before the click.
		c.set_hovered(true, e.pointer_id);

		if (c.on_mouse_button(mouse_button_event{e.is_down, e.pos - c.rect().p, e.button, e.pointer_id})) {
			ASSERT(this->mouse_capture_map.find(e.pointer_id) == this->mouse_capture_map.end())

			// normally, we get here only when the mouse was not captured by widget, because
			// as soon as mouse button down event comes to some widget, it captures the mouse.
			// So, the event should be button down.
			// But, in theory, it can be button up event here, if some widget which captured
			// mouse was removed from its parent. So, check if we have button down event.
			if (e.is_down) {
				this->mouse_capture_map.insert(
					std::make_pair(e.pointer_id, mouse_capture_info{utki::make_weak(i->to_shared_ptr()), 1})
				);
			}

			// widget has consumed the mouse button event,
			// that means the rest of the underlying widgets are not hovered,
			// update the hovered state of those
			for (++i; i != this->children().rend(); ++i) {
				i->get().set_hovered(false, e.pointer_id);
			}

			return true;
		}
	}

	return this->widget::on_mouse_button(e);
}

bool container::on_mouse_move(const mouse_move_event& e)
{
	//	TRACE(<< "container::on_mouse_move(): pos = " << pos << std::endl)

	blocked_flag_guard blocked_guard(this->is_blocked);

	// check if mouse captured
	if (!e.ignore_mouse_capture) {
		auto i = this->mouse_capture_map.find(e.pointer_id);
		if (i != this->mouse_capture_map.end()) {
			if (auto w = i->second.capturing_widget.lock()) {
				if (w->is_interactive()) {
					w->on_mouse_move(mouse_move_event{e.pos - w->rect().p, e.pointer_id, e.ignore_mouse_capture});
					w->set_hovered(w->rect().overlaps(e.pos), e.pointer_id);

					// doesn't matter what to return because parent widget also captured
					// the mouse and in this case the return value is ignored
					return true;
				}
			}
			this->mouse_capture_map.erase(i);
		}
	}

	// call children in reverse order
	for (auto i = this->children().rbegin(); i != this->children().rend(); ++i) {
		auto& c = i->get();

		if (!c.is_interactive()) {
			ASSERT(!c.is_hovered(), [&](auto& o) {
				o << "c->name() = " << c.id();
			})
			continue;
		}

		if (!c.rect().overlaps(e.pos)) {
			c.set_hovered(false, e.pointer_id);
			continue;
		}

		c.set_hovered(true, e.pointer_id);

		// LOG("e.pos = " << e.pos << ", rect() = " << c->rect() << std::endl)
		if (c.on_mouse_move(mouse_move_event{e.pos - c.rect().p, e.pointer_id, e.ignore_mouse_capture})) {
			// widget has consumed the mouse move event,
			// that means the rest of the underlying widgets are not hovered,
			// update the hovered state of those
			for (++i; i != this->children().rend(); ++i) {
				i->get().set_hovered(false, e.pointer_id);
			}

			return true;
		}
	}

	return false;
}

void container::on_hovered_change(unsigned pointer_id)
{
	if (this->is_hovered(pointer_id)) {
		return;
	}

	// un-hover all the children since container became un-hovered
	blocked_flag_guard blocked_guard(this->is_blocked);
	for (auto& w : this->children()) {
		w.get().set_hovered(false, pointer_id);
	}
}

vector2 container::measure(const vector2& quotum) const
{
	return this->get_layout().measure(quotum, this->children());
}

void container::on_lay_out()
{
	this->get_layout().lay_out(
		this->rect().d, //
		this->children()
	);
}

void container::set_widget_parent(widget& w)
{
	w.parent_container = this;
	w.on_parent_change();
}

widget_list::const_iterator container::insert_internal(
	utki::shared_ref<widget> w, //
	widget_list::const_iterator before
)
{
	if (this->is_blocked) {
		throw std::logic_error("container::insert(): children list is locked");
	}

	if (before != this->children().end() && before->get().parent() != this) {
		throw std::invalid_argument("container::insert(): given 'before' iterator points to a different container");
	}

	widget& ww = w.get();

	if (ww.parent_container) {
		throw std::invalid_argument("container::insert(): the widget is already added to some other container");
	}

	if (ww.context.to_shared_ptr() != this->context.to_shared_ptr()) {
		throw std::invalid_argument("container::insert(): cannot insert widget from another GUI context");
	}

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
	auto ret = this->children_list.variable.emplace(
		before, //
		std::move(w)
	);

	this->set_widget_parent(ww);

	ASSERT(!ww.is_hovered())
	return ret;
}

widget_list::const_iterator container::insert(
	utki::shared_ref<widget> w, //
	widget_list::const_iterator before
)
{
	auto ret = this->insert_internal(
		std::move(w), //
		before
	);

	this->on_children_change();
	return ret;
}

widget_list::const_iterator container::push_back(utki::shared_ref<widget> w)
{
	return this->insert(
		std::move(w), //
		this->children().end()
	);
}

void container::push_back_internal(utki::span<const utki::shared_ref<widget>> ww)
{
	for (const auto& w : ww) {
		this->insert_internal(
			w, //
			this->children().end()
		);
	}
}

void container::push_back(utki::span<const utki::shared_ref<widget>> ww)
{
	this->push_back_internal(ww);
	this->on_children_change();
}

widget_list::const_iterator container::erase(widget_list::const_iterator child)
{
	if (this->is_blocked) {
		throw std::logic_error("container::erase(): children list is locked");
	}

	if (child == this->children().end()) {
		throw std::invalid_argument("container::erase(): given 'child' iterator is invalid");
	}

	if (child->get().parent() != this) {
		throw std::invalid_argument("container::erase(): given child widget belongs to a different container");
	}

	auto w = *child;

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
	auto ret = this->children_list.variable.erase(child);

	w.get().parent_container = nullptr;
	w.get().set_unhovered();
	w.get().on_parent_change();

	this->on_children_change();

	return ret;
}

widget_list::const_reverse_iterator container::erase(widget_list::const_reverse_iterator child)
{
	return widget_list::const_reverse_iterator( //
		this->erase(
			// the base iterator points to the next element to the one the reverse iterator points, so use std::prev()
			std::prev(child.base())
		)
	);
}

void container::pop_back()
{
	if (this->children().empty()) {
		return;
	}
	this->erase(std::prev(this->children().end()));
}

void container::clear()
{
	for (auto i = this->children().begin(); i != this->children().end(); i = this->erase(i)) {
	}
}

std::shared_ptr<widget> container::try_get_widget(
	std::string_view id, //
	bool allow_itself
) noexcept
{
	if (auto r = this->widget::try_get_widget(id, allow_itself)) {
		return r;
	}

	for (auto& w : this->children()) {
		if (auto r = w.get().widget::try_get_widget(id, true)) {
			return r;
		}
	}
	for (auto& w : this->children()) {
		if (auto r = w.get().try_get_widget(id, false)) {
			return r;
		}
	}
	return nullptr;
}

widget_list::const_iterator container::change_child_z_position(
	widget_list::const_iterator child, //
	widget_list::const_iterator before
)
{
	if (this->is_blocked) {
		throw std::logic_error("container::change_child_z_position(): children list is locked");
	}

	if (child == this->children().end()) {
		throw std::invalid_argument("container::change_child_z_position(): given 'child' iterator is invalid");
	}

	if (child->get().parent() != this) {
		throw std::invalid_argument(
			"container::change_child_z_position(): given child widget belongs to a different container"
		);
	}

	if (before != this->children().end() && before->get().parent() != this) {
		throw std::invalid_argument(
			"container::change_child_z_position(): given 'before' iterator points to a different container"
		);
	}

	if (child == before) {
		// child is already at the right place
		return child;
	}

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
	auto b = this->children_list.variable.erase(before, before); // remove constness
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
	auto i = this->children_list.variable.erase(child, child); // remove constness

	decltype(child) ret;

	auto distance = std::distance(i, b);
	ASSERT(distance != 0)
	if (distance > 0) {
		ret = std::rotate(i, std::next(i), b);
	} else {
		ret = std::rotate(b, i, std::next(i));
		--ret;
	}

	this->on_children_change();

	return ret;
}

widget_list::const_iterator container::find(const widget& w)
{
	return std::find_if(
		this->children().begin(),
		this->children().end(),
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
		[&w](const decltype(this->children_list.variable)::value_type& v) -> bool {
			return &v.get() == &w;
		}
	);
}

const_widget_list::const_iterator container::find(const widget& w) const
{
	return std::find_if(
		this->children().begin(),
		this->children().end(),
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
		[&w](const decltype(this->children_list.constant)::value_type& v) -> bool {
			return &v.get() == &w;
		}
	);
}

void container::on_enabled_change()
{
	blocked_flag_guard blocked_guard(this->is_blocked);

	for (auto& c : this->children()) {
		c.get().set_enabled(this->is_enabled());
	}
}

void container::on_reload()
{
	for (auto& c : this->children()) {
		c.get().reload();
	}
}

utki::shared_ref<ruis::container> ruis::make::container(
	utki::shared_ref<ruis::context> context, //
	container::all_parameters params,
	widget_list children
)
{
	return utki::make_shared<ruis::container>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}

utki::shared_ref<ruis::container> ruis::make::pile(
	utki::shared_ref<ruis::context> context, //
	widget::all_parameters params,
	widget_list children
)
{
	// clang-format off
	return make::container(
		std::move(context),
		{
		 	.layout_params = std::move(params.layout_params),
		 	.widget_params = std::move(params.widget_params),
		 	.container_params{
				.layout = layout::pile
			}
		},
		std::move(children)
	);
	// clang-format on
}

utki::shared_ref<ruis::container> ruis::make::column(
	utki::shared_ref<ruis::context> context, //
	widget::all_parameters params,
	widget_list children
)
{
	// clang-format off
	return make::container(
		std::move(context),
		{
		 	.layout_params = std::move(params.layout_params),
		 	.widget_params = std::move(params.widget_params),
		 	.container_params{
				.layout = layout::column
			}
		},
		std::move(children)
	);
	// clang-format on
}

utki::shared_ref<ruis::container> ruis::make::row(
	utki::shared_ref<ruis::context> context, //
	widget::all_parameters params,
	widget_list children
)
{
	// clang-format off
	return make::container(
		std::move(context),
		{
		 	.layout_params = std::move(params.layout_params),
		 	.widget_params = std::move(params.widget_params),
		 	.container_params{
				.layout = layout::row
			}
		},
		std::move(children)
	);
	// clang-format on
}
