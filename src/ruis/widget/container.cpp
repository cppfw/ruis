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

#include "container.hpp"

#include <utki/config.hpp>
#include <utki/debug.hpp>

#include "../context.hpp"
#include "../util/to_tml.hpp"
#include "../util/util.hpp"

using namespace std::string_literals;

using namespace ruis;

container::container(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	widget_list children
) :
	widget( //
		context,
		std::move(params.layout_params),
		std::move(params.widget)
	),
	children_list(std::move(children)),
	layout([&]() {
		if (!params.params.layout) {
			return ruis::layout::trivial;
		}
		return utki::shared_ref(params.params.layout);
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
	const mat4& matrix, //
	const widget& c
) const
{
	if (!c.is_visible()) {
		return;
	}

	ruis::mat4 matr(matrix);
	matr.translate(c.rect().p);

	c.render_internal(matr);
}

void container::render(const ruis::mat4& matrix) const
{
	for (auto& w : this->children()) {
		this->render_child(matrix, w.get());
	}
}

event_status container::on_mouse_button(const mouse_button_event& e)
{
	blocked_flag_guard blocked_guard(this->is_blocked);

	// check if mouse captured
	{
		auto i = this->mouse_capture_map.find(e.pointer_id);
		if (i != this->mouse_capture_map.end()) {
			if (auto w = i->second.capturing_widget.lock()) {
				if (w->is_interactive()) {
					w->on_mouse_button(mouse_button_event{
						.action = e.action, //
						.pos = e.pos - w->rect().p,
						.button = e.button,
						.pointer_id = e.pointer_id
					});
					w->set_hovered(
						w->rect().overlaps(e.pos), //
						e.pointer_id
					);

					unsigned& num_buttons_captured = i->second.num_buttons_captured;
					if (e.action == button_action::press) {
						// if we get button down event for mouse capturing widget, then it is not for the buttons
						// which are already down, so we increase the button counter.
						++num_buttons_captured;
					} else {
						// If we get button up event, then it is for one of the buttons which was down before,
						// so we decrease the button counter.
						--num_buttons_captured;
					}
					if (num_buttons_captured == 0) {
						this->mouse_capture_map.erase(i);
					}

					// TODO: make same as in on_mouse_move handler? I.e. returning the actual status.
					// doesn't matter what to return because parent widget also captured
					// the mouse and in this case the return value is ignored
					return event_status::consumed;
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

		if (c.on_mouse_button(mouse_button_event{
				.action = e.action, //
				.pos = e.pos - c.rect().p,
				.button = e.button,
				.pointer_id = e.pointer_id
			}) == event_status::consumed)
		{
			utki::assert(
				[&]() {
					return this->mouse_capture_map.find(e.pointer_id) == this->mouse_capture_map.end();
				},
				SL
			);

			// Normally, we get here only when the mouse was not captured by widget, because
			// as soon as mouse button down event comes to some widget, it captures the mouse.
			// So, the event should be button down.
			// But, in theory, it can be button up event here, if some widget which captured
			// mouse was removed from its parent. So, check if we have button down event.
			if (e.action == button_action::press) {
				this->mouse_capture_map.insert(std::make_pair(
					e.pointer_id, //
					mouse_capture_info{
						.capturing_widget = utki::make_weak(i->to_shared_ptr()), //
						.num_buttons_captured = 1 // one button captured
					}
				));
			}

			// Widget has consumed the mouse button event,
			// that means the rest of the underlying widgets are not hovered,
			// update the hovered state of those.
			for (++i; i != this->children().rend(); ++i) {
				i->get().set_hovered(
					false, //
					e.pointer_id
				);
			}

			return event_status::consumed;
		}
	}

	return this->widget::on_mouse_button(e);
}

event_status container::on_mouse_move(const mouse_move_event& e)
{
	blocked_flag_guard blocked_guard(this->is_blocked);

	// check if mouse captured
	if (!e.ignore_mouse_capture) {
		if (auto i = this->mouse_capture_map.find(e.pointer_id); i != this->mouse_capture_map.end()) {
			if (auto w = i->second.capturing_widget.lock()) {
				if (w->is_interactive()) { // TODO: why check interactive here?
					auto status = w->on_mouse_move(mouse_move_event{
						.pos = e.pos - w->rect().p, //
						.pointer_id = e.pointer_id,
						.ignore_mouse_capture = e.ignore_mouse_capture
					});
					w->set_hovered(
						w->rect().overlaps(e.pos), //
						e.pointer_id
					);

					// Need to return actual status here because some widgets may analyse it
					// to implement some specific behaviour, e.g. scrolling by dragging the mouse.
					return status;
				}
			}
			this->mouse_capture_map.erase(i);
		}
	}

	// call children in reverse order
	for (auto i = this->children().rbegin(); i != this->children().rend(); ++i) {
		auto& c = i->get();

		if (!c.is_interactive()) {
			utki::assert(
				!c.is_hovered(),
				[&](auto& o) {
					o << "c->name() = " << c.id();
				},
				SL
			);
			continue;
		}

		if (!c.rect().overlaps(e.pos)) {
			c.set_hovered(false, e.pointer_id);
			continue;
		}

		c.set_hovered(true, e.pointer_id);

		if (c.on_mouse_move(mouse_move_event{
				.pos = e.pos - c.rect().p, //
				.pointer_id = e.pointer_id,
				.ignore_mouse_capture = e.ignore_mouse_capture
			}) == event_status::consumed)
		{
			// widget has consumed the mouse move event,
			// that means the rest of the underlying widgets are not hovered,
			// update the hovered state of those
			for (++i; i != this->children().rend(); ++i) {
				i->get().set_hovered(false, e.pointer_id);
			}

			return event_status::consumed;
		}
	}

	return event_status::propagate;
}

void container::on_hovered_change(unsigned pointer_id)
{
	if (this->is_hovered(pointer_id)) {
		return;
	}

	// the container has just became unhovered
	utki::assert(
		[&]() {
			return !this->is_hovered(pointer_id);
		},
		SL
	);

	// unhover all the children
	blocked_flag_guard blocked_guard(this->is_blocked);
	for (auto& w : this->children()) {
		w.get().set_hovered(false, pointer_id);
	}
}

vec2 container::measure(
	const vec2& quotum, //
	const r4::vector2<measure_mode>& mode
) const
{
	return this->get_layout().measure(
		quotum, //
		mode,
		this->children()
	);
}

void container::on_lay_out()
{
	this->get_layout().lay_out(
		this->rect().d, //
		this->children()
	);
}

void container::invalidate_subtree_layout() noexcept
{
	this->widget::invalidate_subtree_layout();
	for (auto& w : this->children()) {
		w.get().invalidate_subtree_layout();
	}
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

	auto w = std::move(*child);

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

namespace {

// a decimal number with at most this many digits
// is guaranteed to fit in uint64_t
constexpr std::size_t max_index_digits = 19;

enum class index_status {
	ok,
	malformed,
	too_big
};

// parse a child index from a tml leaf
// a value with more than max_index_digits digits is reported as too_big
// (it is certainly out of bounds for any child list, and tml::leaf::to_uint64()
// silently returns 0 on overflow instead of throwing);
// any other value is parsed with tml::leaf::to_uint64(),
// which throws std::invalid_argument for non-numeric values -
// in that case the value is reported as malformed
index_status parse_child_index(const tml::leaf& l, std::size_t& index)
{
	// review: no need to check for this, because tml::leaf::to_uint64() throws std::invalid_argument for non-numeric values
	if (l.string.size() > max_index_digits) {
		return index_status::too_big;
	}
	try {
		index = l.to_uint64();
	} catch (const std::exception&) {
		return index_status::malformed;
	}
	return index_status::ok;
}

// single section reporting a malformed 'for' command:
// ruis::container { error { malformed_command } }
widget::probe_response_set make_malformed_command_error()
{
	widget::probe_response_set ret;
	ret.insert(tml::tree(
		utki::demangle(typeid(container)), //
		{tml::tree("error"s, {tml::tree("malformed_command"s)})}
	));
	return ret;
}

// single section reporting a failed index path of the 'for' command:
// ruis::container { error { index { <path prefix> } <reason> [<size>] } }
widget::probe_response_set make_for_error(
	const tml::forest& path_prefix, //
	const char* reason, //
	const tml::tree* size = nullptr
)
{
	tml::forest err_children{tml::tree("index"s, path_prefix)};
	err_children.emplace_back(tml::leaf(reason));
	if (size) {
		err_children.push_back(*size);
	}
	widget::probe_response_set ret;
	ret.insert(tml::tree(
		utki::demangle(typeid(container)), //
		{tml::tree("error"s, std::move(err_children))}
	));
	return ret;
}

// 'for' command: run a single probe command on another widget in the tree,
// addressed by a path of child indices.
// Command format: for { index { <i0> <i1> ... } <command> }
// i0 is an index into the children of the container the command is run on,
// i1 is an index into the children of the i0-th child, and so on.
// The command is run on the widget at the end of the path and its probe
// response is returned as is.
widget::probe_response_set probe_for(container& c, const tml::tree& command)
{
	// the 'for' node must have exactly two children:
	// an 'index' node and a single command node
	const tml::tree* index_node = nullptr;
	const tml::tree* command_node = nullptr;
	for (const auto& child : command.children) {
		if (child.value == "index") {
			if (index_node) {
				return make_malformed_command_error();
			}
			index_node = &child;
		} else {
			if (command_node) {
				return make_malformed_command_error();
			}
			command_node = &child;
		}
	}
	if (!index_node || !command_node) {
		return make_malformed_command_error();
	}

	// walk the index path, starting from this container
	widget* w = &c;
	tml::forest path; // the index nodes of the walked path
	for (const auto& idx_node : index_node->children) {
		std::size_t i = 0;
		const auto status = parse_child_index(idx_node.value, i);
		if (status == index_status::malformed) {
			return make_malformed_command_error();
		}
		path.push_back(idx_node);

		auto* cont = dynamic_cast<container*>(w);
		if (!cont) {
			return make_for_error(path, "not_a_container");
		}
		// a too_big index is certainly out of bounds
		if (status == index_status::too_big || i >= cont->size()) {
			const auto size_node = to_tml(cont->size());
			return make_for_error(path, "out_of_bound", &size_node);
		}
		w = &cont->children()[i].get();
	}

	return w->probe(*command_node);
}

} // namespace

container::probe_response_set container::probe(const tml::tree& command)
{
	if (command.value == "get_children") {
		probe_response_set ret;
		ret.insert(tml::tree(
			utki::demangle(typeid(container)), //
			{tml::tree("children_count"s, {to_tml(this->size())})}
		));
		return ret;
	}
	if (command.value == "for") {
		return probe_for(*this, command);
	}
	// all other commands are delegated to the base implementation
	return widget::probe(command);
}

utki::shared_ref<ruis::container> ruis::make::container(
	const utki::shared_ref<ruis::context>& context, //
	container::all_parameters params,
	widget_list children
)
{
	return utki::make_shared<ruis::container>(
		context, //
		std::move(params),
		std::move(children)
	);
}

utki::shared_ref<ruis::container> ruis::make::pile(
	const utki::shared_ref<ruis::context>& context, //
	widget::all_parameters params,
	widget_list children
)
{
	// clang-format off
	return make::container(
		context,
		{
			.layout_params = std::move(params.layout_params),
			.widget = std::move(params.widget),
			.params{
				.layout = layout::pile
			}
		},
		std::move(children)
	);
	// clang-format on
}

utki::shared_ref<ruis::container> ruis::make::column(
	const utki::shared_ref<ruis::context>& context, //
	widget::all_parameters params,
	widget_list children
)
{
	// clang-format off
	return make::container(
		context,
		{
			.layout_params = std::move(params.layout_params),
			.widget = std::move(params.widget),
			.params{
				.layout = layout::column
			}
		},
		std::move(children)
	);
	// clang-format on
}

utki::shared_ref<ruis::container> ruis::make::row(
	const utki::shared_ref<ruis::context>& context, //
	widget::all_parameters params,
	widget_list children
)
{
	// clang-format off
	return make::container(
		context,
		{
			.layout_params = std::move(params.layout_params),
			.widget = std::move(params.widget),
			.params{
				.layout = layout::row
			}
		},
		std::move(children)
	);
	// clang-format on
}
