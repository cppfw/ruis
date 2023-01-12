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

#include "../widget.hpp"
#include "../base/single_line_text_widget.hpp"

#include "../../updateable.hpp"
#include "../character_input_widget.hpp"

namespace morda{

/**
 * @brief TODO:.
 */
class text_input_line :
		public single_line_text_widget,
		public character_input_widget,
		public color_widget,
		private updateable
{
	size_t firstVisibleCharIndex = 0;
	real xOffset = 0;

	real cursorPos;

	size_t cursorIndex = 0;

	real selectionStartPos;

	size_t selectionStartIndex = 0;

	bool cursorBlinkVisible;

	bool ctrlPressed;
	bool shiftPressed;

	bool leftMouseButtonDown = false;

public:
	text_input_line(const text_input_line&) = delete;
	text_input_line& operator=(const text_input_line&) = delete;

	text_input_line(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	virtual ~text_input_line()noexcept{}

	vector2 measure(const morda::vector2& quotum)const noexcept override;

	void render(const morda::matrix4& matrix) const override;

	bool on_mouse_button(const mouse_button_event& event)override;

	bool on_mouse_move(const mouse_move_event& event)override;

	void on_focus_change()override;

	bool on_key(const morda::key_event& e)override;

	void on_resize()override;

	void update(uint32_t dt)override;

	void on_character_input(const character_input_event& e)override;

	void set_cursor_index(size_t index, bool selection = false);

private:
	void updateCursorPosBasedOnIndex();

	void startCursorBlinking();

	size_t posToIndex(real pos);

	real indexToPos(size_t index);

	bool thereIsSelection()const noexcept{
		return this->cursorIndex != this->selectionStartIndex;
	}

	//returns new cursor index
	size_t deleteSelection();
};

}
