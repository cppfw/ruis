/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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
#include "../../widget.hpp"
#include "../base/single_line_text_widget.hpp"
#include "../character_input_widget.hpp"

namespace morda {

/**
 * @brief TODO:.
 */
class text_input_line :
	public single_line_text_widget,
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

	text_input_line(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);

	~text_input_line() override = default;

	vector2 measure(const morda::vector2& quotum) const noexcept override;

	void render(const morda::matrix4& matrix) const override;

	bool on_mouse_button(const mouse_button_event& event) override;

	bool on_mouse_move(const mouse_move_event& event) override;

	void on_focus_change() override;

	bool on_key(const morda::key_event& e) override;

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

} // namespace morda
