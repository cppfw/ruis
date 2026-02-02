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

#pragma once

#include "../../updateable.hpp"
#include "../base/text_string_widget.hpp"
#include "../widget.hpp"

#include "character_input_widget.hpp"

namespace ruis {

/**
 * @brief TODO:.
 */
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via text_string_widget")
class text_input_line :
	public text_string_widget,
	public character_input_widget,
	public color_widget,
	private updateable
{
	size_t first_visible_char_index = 0;
	real x_offset = 0;

	real cursor_pos = 0;

	size_t cursor_index = 0;

	real selection_start_pos = 0;

	size_t selection_start_index = 0;

	bool cursor_blink_visible = true;

	bool ctrl_pressed = false;
	bool shift_pressed = false;

	bool left_mouse_button_down = false;

public:
	text_input_line(const text_input_line&) = delete;
	text_input_line& operator=(const text_input_line&) = delete;

	text_input_line(text_input_line&&) = delete;
	text_input_line& operator=(text_input_line&&) = delete;

	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		text_widget::parameters text_widget_params;
		color_widget::parameters color_params;
	};

	text_input_line(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		string text
	);

	~text_input_line() override = default;

	vec2 measure(const ruis::vec2& quotum) const noexcept override;

	void render(const ruis::mat4& matrix) const override;

	event_status on_mouse_button(const mouse_button_event& event) override;
	event_status on_mouse_move(const mouse_move_event& event) override;

	void on_focus_change() override;

	event_status on_key(const ruis::key_event& e) override;

	void on_resize() override;

	void update(uint32_t dt) override;

	void on_character_input(const character_input_event& e) override;

	void set_cursor_index(size_t index, bool selection = false);

private:
	void update_cursor_pos_based_on_index();

	void start_cursor_blinking();

	size_t pos_to_index(real pos);

	real index_to_pos(size_t index);

	bool there_is_selection() const noexcept
	{
		return this->cursor_index != this->selection_start_index;
	}

	// returns new cursor index
	size_t delete_selection();
};

namespace make {
utki::shared_ref<ruis::text_input_line> text_input_line(
	utki::shared_ref<ruis::context> context, //
	ruis::text_input_line::all_parameters params,
	ruis::string text
);
} // namespace make

} // namespace ruis
