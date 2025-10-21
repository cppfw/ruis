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

#include "gui.hpp"

#include "res/tml.hpp"
#include "widget/input/character_input_widget.hpp"
#include "widget/label/gap.hpp"

using namespace ruis;

// NOLINTNEXTLINE(modernize-pass-by-value)
gui::gui(utki::shared_ref<ruis::context> context) :
	context(std::move(context)),
	root_widget(ruis::make::gap(this->context, {}))
{}

void gui::init_standard_widgets(papki::file& fi)
{
	// mount default resource pack

	std::vector<std::string> paths;

	if (fi.path().length() != 0) {
		paths.push_back(fi.path());
	}

	paths.emplace_back("ruis_res/");

#if (M_OS == M_OS_LINUX && M_OS_NAME != M_OS_NAME_ANDROID) || (M_OS == M_OS_MACOSX && M_OS_NAME != M_OS_NAME_IOS) || \
	(M_OS == M_OS_UNIX)

	unsigned soname =
#	include "../soname.txt"
		;

	{
		std::stringstream ss;
		ss << "/usr/local/share/ruis/res" << soname << "/";
		paths.push_back(ss.str());
	}
	{
		std::stringstream ss;
		ss << "/usr/share/ruis/res" << soname << "/";
		paths.push_back(ss.str());
	}
#endif

	bool mounted = false;
	for (const auto& s : paths) {
		try {
			fi.set_path(s);
			this->context.get().loader().mount_res_pack(fi);
		} catch (std::runtime_error&) {
			// utki::logcat_debug("could not mount respack: ", s, '\n');
			continue;
		}

		fi.set_path(utki::cat(s, "style_dark.tml"));
		this->context.get().style_provider.get().set(utki::make_shared<style_sheet>(tml::read(fi)));

		mounted = true;
		break;
	}

	if (!mounted) {
		throw std::runtime_error("gui::init_standard_widgets(): could not mount default resource pack");
	}
}

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

	this->root_widget.get().move_to(ruis::vector2(0));
	this->root_widget.get().resize(this->viewport_rect.d);
}

void gui::render(const matrix4& matrix) const
{
	if (this->get_root().is_layout_dirty()) {
		LOG([](auto& o) {
			o << "root widget re-layout needed!" << std::endl;
		})
		// TODO: render() is const, but calls non-const lay_out(), fix it somehow? Perhaps make render() non-const?
		this->root_widget.get().lay_out();
	}

	ruis::matrix4 m = make_viewport_matrix(
		matrix, //
		this->viewport_rect.d
	);

	this->get_root().render_internal(m);
}

void gui::send_mouse_move(
	const vector2& pos, //
	unsigned id
)
{
	auto& rw = this->get_root();
	if (rw.is_interactive()) {
		rw.set_hovered(rw.rect().overlaps(pos), id);
		rw.on_mouse_move(mouse_move_event{pos, id, false});
	}
}

void gui::send_mouse_button(
	bool is_down, //
	const vector2& pos,
	mouse_button button,
	unsigned id
)
{
	auto& rw = this->get_root();
	if (rw.is_interactive()) {
		rw.set_hovered(rw.rect().overlaps(pos), id);
		rw.on_mouse_button(mouse_button_event{is_down, pos, button, id});
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
	bool is_down, //
	key key_code
)
{
	//		TRACE(<< "HandleKeyEvent(): is_down = " << is_down << " is_char_input_only = " << is_char_input_only << "
	// keyCode = " << unsigned(keyCode) << std::endl)

	auto modifier = ruis::to_key_modifier(key_code);
	if (modifier != ruis::key_modifier::unknown) {
		this->key_modifiers.set(modifier, is_down);
	}

	ruis::key_event e;
	e.combo.key = key_code;
	e.combo.modifiers = this->key_modifiers;
	e.is_down = is_down;

	if (auto w = this->context.get().focused_widget.lock()) {
		//		TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		w->on_key_internal(e);
	} else {
		//		TRACE(<< "HandleKeyEvent(): there is no focused widget, passing to rootWidget" << std::endl)
		this->get_root().on_key_internal(e);
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
