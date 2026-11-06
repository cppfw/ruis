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

#include "gui.hpp"

#include <exception>
#include <ranges>
#include <string>

#include <tml/crawler.hpp>
#include <utki/debug.hpp>
#include <utki/unicode.hpp>

#include "res/tml.hpp"
#include "util/events.hpp"
#include "util/key.hpp"
#include "util/util.hpp"
#include "widget/input/base/character_input_widget.hpp"
#include "widget/label/gap.hpp"

using namespace ruis;

// NOLINTNEXTLINE(modernize-pass-by-value)
gui::gui(utki::shared_ref<ruis::context> context) :
	context(std::move(context)),
	root_widget(ruis::make::gap(this->context, {}))
{}

void gui::set_viewport(const ruis::rect& rect)
{
	if (this->viewport_rect == rect) {
		return;
	}

	this->context.get().ren().ctx().apply([&]() {
		this->viewport_rect = rect;

		utki::log_debug([&](auto& o) {
			o << "gui::set_viewport(): new viewport retangle = " << this->viewport_rect << std::endl;
		});

		this->context.get().ren().ctx().set_viewport(this->viewport_rect.to<uint32_t>());

		this->root_widget.get().resize(this->viewport_rect.d);
	});
}

void gui::set_root(utki::shared_ref<ruis::widget> w)
{
	if (w.get().parent()) {
		throw std::invalid_argument("given widget is already added to some container");
	}

	if (w.get().context.to_shared_ptr() != this->context.to_shared_ptr()) {
		throw std::invalid_argument("cannot set root widget from another GUI context");
	}

	this->root_widget = std::move(w);

	this->root_widget.get().move_to(ruis::vec2(0));
	this->root_widget.get().resize(this->viewport_rect.d);
}

void gui::render(const mat4& matrix) const
{
	if (this->get_root().is_layout_dirty()) {
		LOG([](auto& o) {
			o << "root widget re-layout needed!" << std::endl;
		})
		// TODO: render() is const, but calls non-const lay_out(), fix it somehow? Perhaps make render() non-const?
		this->root_widget.get().lay_out();
	}

	ruis::mat4 m = make_viewport_matrix(
		matrix, //
		this->viewport_rect.d
	);

	this->get_root().render_internal(m);
}

void gui::send_mouse_move(
	const vec2& pos, //
	unsigned id
)
{
	auto& rw = this->get_root();
	if (rw.is_interactive()) {
		rw.set_hovered(rw.rect().overlaps(pos), id);
		rw.on_mouse_move(mouse_move_event{
			.pos = pos, //
			.pointer_id = id,
			.ignore_mouse_capture = false
		});
	}
}

void gui::send_mouse_button(
	button_action action, //
	const vec2& pos,
	mouse_button button,
	unsigned id
)
{
	auto& rw = this->get_root();
	if (rw.is_interactive()) {
		rw.set_hovered(rw.rect().overlaps(pos), id);
		rw.on_mouse_button(mouse_button_event{
			.action = action, //
			.pos = pos,
			.button = button,
			.pointer_id = id
		});
	}
}

void gui::send_mouse_hover(
	bool is_hovered, //
	unsigned pointer_id
)
{
	this->get_root().set_hovered(is_hovered, pointer_id);
}

void gui::send_key(
	button_action action, //
	key key_code
)
{
	auto modifier = ruis::to_key_modifier(key_code);
	if (modifier != ruis::key_modifier::unknown) {
		this->key_modifiers.set(modifier, action == button_action::press);
	}

	ruis::key_event e;
	e.combo.key = key_code;
	e.combo.modifiers = this->key_modifiers;
	e.action = action;

	bool consumed = false;
	if (auto w = this->context.get().focused_widget.lock()) {
		//		TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		consumed = w->on_key_internal(e);
	}

	// If the key event was not consumed, i.e. there is no focused widget, or the focused
	// widget (and its ancestors up to the root widget) did not consume it, route it to
	// the default key handler.
	if (!consumed && this->default_key_handler) {
		this->default_key_handler(e);
	}
}

void gui::send_character_input(
	const input_string_provider& string_provider, //
	key key_code
)
{
	if (auto w = this->context.get().focused_widget.lock()) {
		if (auto c = dynamic_cast<character_input_widget*>(w.get())) {
			character_input_event e;
			auto str = string_provider.get();
			e.string = str;
			e.combo.key = key_code;
			e.combo.modifiers = this->key_modifiers;

			c->on_character_input(e);
		}
	}
}

namespace {

// an input string provider that returns a fixed string (for send_character_input)
struct fixed_input_string_provider : public gui::input_string_provider {
	std::u32string str;

	std::u32string get() const override
	{
		return this->str;
	}
};

// make a single-section 'ruis::gui' probe response forest with the given children
tml::forest make_gui_response(tml::forest children)
{
	return {tml::tree(tml::leaf(utki::demangle(typeid(gui))), std::move(children))};
}

// success response: ruis::gui { ok }
tml::forest make_gui_ok()
{
	return make_gui_response({tml::tree(tml::leaf("ok"))});
}

// error response: ruis::gui { error { <diagnostic> } }
tml::forest make_gui_error(std::string diagnostic)
{
	const tml::forest children{tml::tree(tml::leaf("error"), tml::forest{tml::tree(tml::leaf(std::move(diagnostic)))})};
	return make_gui_response(std::move(children));
}

// flatten the widget probe response (a set of sections) to a forest
tml::forest flatten(const widget::probe_response_set& resp)
{
	return {resp.begin(), resp.end()};
}

} // namespace

tml::forest gui::probe(const tml::tree& command)
{
	// The parameters are read with the tml::crawler cascades, so a missing parameter
	// or a malformed parameter value is detected by the exceptions thrown by the crawler
	// (or by the tml::leaf numeric conversions), which are caught and turned into an
	// error response carrying the exception message as the diagnostic.
	if (command.value == "send_mouse_move") {
		try {
			const vec2 pos = make_vec2(tml::const_crawler(command.children).to("pos").get().children);
			const unsigned id = unsigned(tml::const_crawler(command.children).to("id").in().get().value.to_uint32());
			this->send_mouse_move(pos, id);
			return make_gui_ok();
		} catch (const std::exception& e) {
			return make_gui_error(e.what());
		}
	} else if (command.value == "send_mouse_button") {
		try {
			const button_action action =
				ruis::to_button_action(tml::const_crawler(command.children).to("action").in().get().value.string);
			if (action == button_action::enum_size) {
				return make_gui_error("malformed_action");
			}
			const vec2 pos = make_vec2(tml::const_crawler(command.children).to("pos").get().children);
			const mouse_button button =
				ruis::to_mouse_button(tml::const_crawler(command.children).to("button").in().get().value.string);
			if (button == mouse_button::enum_size) {
				return make_gui_error("malformed_button");
			}
			const unsigned id = unsigned(tml::const_crawler(command.children).to("id").in().get().value.to_uint32());
			this->send_mouse_button(action, pos, button, id);
			return make_gui_ok();
		} catch (const std::exception& e) {
			return make_gui_error(e.what());
		}
	} else if (command.value == "send_key") {
		try {
			const button_action action =
				ruis::to_button_action(tml::const_crawler(command.children).to("action").in().get().value.string);
			if (action == button_action::enum_size) {
				return make_gui_error("malformed_action");
			}
			const ruis::key k = ruis::to_key(tml::const_crawler(command.children).to("key").in().get().value.string);
			if (k == ruis::key::unknown) {
				return make_gui_error("malformed_key");
			}
			this->send_key(action, k);
			return make_gui_ok();
		} catch (const std::exception& e) {
			return make_gui_error(e.what());
		}
	} else if (command.value == "send_character_input") {
		std::string str;
		ruis::key k = ruis::key::unknown;
		// the key parameter is optional, so check its presence separately:
		// the crawler's to() throws if the parameter is absent
		const bool has_key = std::ranges::any_of(
			command.children, //
			[](const auto& c) {
				return c.value.string == "key";
			}
		);
		try {
			str = std::string(tml::const_crawler(command.children).to("string").in().get().value.string);
			if (has_key) {
				k = ruis::to_key(tml::const_crawler(command.children).to("key").in().get().value.string);
				if (k == ruis::key::unknown) {
					return make_gui_error("malformed_key");
				}
			}
		} catch (const std::exception& e) {
			return make_gui_error(e.what());
		}
		fixed_input_string_provider provider;
		provider.str = utki::to_utf32(str);
		this->send_character_input(provider, k);
		return make_gui_ok();
	} else if (command.value == "send_to_root") {
		if (command.children.empty()) {
			return make_gui_error("malformed_command");
		}
		return flatten(this->get_root().probe(command.children.front()));
	} else if (command.value == "send_to_focused") {
		if (command.children.empty()) {
			return make_gui_error("malformed_command");
		}
		if (auto w = this->context.get().focused_widget.lock()) {
			return flatten(w->probe(command.children.front()));
		}
		return make_gui_error("no_focused_widget");
	}

	// Unknown commands result in an empty response.
	return {};
}
