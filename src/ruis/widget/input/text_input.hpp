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

#include "base/character_input_widget.hpp"

namespace ruis {

/**
 * @brief Text input field widget without any decoration.
 * The user can type into the field, move the cursor with the keyboard or the mouse,
 * and select a range of characters. When the field has keyboard focus it also receives
 * character input events. If the field text is empty, the hint (placeholder) text is
 * rendered instead.
 */
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via text_string_widget")
class text_input :
	public text_string_widget, //
	public character_input_widget,
	private updateable // TODO: make it virtual-inherited?
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
	/**
	 * @brief Kind of the keyboard to use for the text input.
	 */
	enum class keyboard_kind {
		/**
		 * @brief General (alphanumeric) keyboard.
		 */
		general,

		/**
		 * @brief Numeric keyboard.
		 */
		numeric
	};

	struct specific_parameters {
		/**
		 * @brief Hint (placeholder) text rendered when the field text is empty.
		 */
		ruis::string hint;

		/**
		 * @brief Color of the hint text.
		 * Defaults to the 'color_text_secondary' style value if undefined.
		 */
		styled<ruis::color> hint_color;

		/**
		 * @brief Kind of the keyboard to use for the text input.
		 * Defaults to general.
		 */
		text_input::keyboard_kind keyboard_kind = text_input::keyboard_kind::general;
		/**
		 * @brief Optional input filter.
		 * If set, it is invoked before an input edit is applied, with:
		 * - the original (current) text,
		 * - the range [replace_start, replace_end) of the original text to be replaced
		 *   (both are equal in case of plain insertion, i.e. when nothing is selected),
		 * - the text to be inserted in place of that range.
		 * If it returns `false`, the input is rejected and the text is left unchanged.
		 */
		std::function<bool(
			std::u32string_view original, //
			size_t replace_start,
			size_t replace_end,
			std::u32string_view to_insert
		)>
			filter;
	};

	struct parameters {
		text_string_widget::parameters text;
		specific_parameters specific;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

private:
	specific_parameters params;

public:
	text_input(const text_input&) = delete;
	text_input& operator=(const text_input&) = delete;

	text_input(text_input&&) = delete;
	text_input& operator=(text_input&&) = delete;

	/**
	 * @brief Construct text input widget.
	 * @param context - ruis context.
	 * @param params - text input widget parameters.
	 * @param text - initial text content.
	 */
	text_input(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params,
		string text
	);

	~text_input() override = default;

	vec2 measure(const ruis::vec2& quotum) const noexcept override;

	void render(const ruis::mat4& matrix) const override;

	event_status on_mouse_button(const mouse_button_event& event) override;
	event_status on_mouse_move(const mouse_move_event& event) override;

	void on_focus_change() override;

	event_status on_key(const ruis::key_event& e) override;

	void on_text_change() override;

	void on_resize() override;

	void update(uint32_t dt) override;

	void on_character_input(const character_input_event& e) override;

	/**
	 * @brief Set cursor position.
	 * @param index - new cursor position, a 0-based index within the text.
	 * @param selection - if true, the selection is extended from the selection anchor
	 *                    to the new cursor position; if false, the selection is collapsed
	 *                    (no selection).
	 * The widget is focused and the cursor blinking is (re)started.
	 */
	// TODO: rename to set_cursor_position
	void set_cursor_index(
		size_t index, //
		bool selection = false
	);

	// TODO: add 'size_t get_cursor_position()'

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
/**
 * @brief Construct text input widget.
 * @param context - ruis context.
 * @param params - text input widget parameters.
 * @param text - initial text content.
 * @return newly constructed text input widget.
 */
utki::shared_ref<ruis::text_input> text_input(
	const utki::shared_ref<ruis::context>& context, //
	ruis::text_input::all_parameters params,
	ruis::string text
);
} // namespace make

} // namespace ruis