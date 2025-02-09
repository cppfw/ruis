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

#pragma once

#include "context.hpp"
#include "updateable.hpp"

namespace ruis {

class gui final
{
	friend class updateable;
	friend class widget;

public:
	const utki::shared_ref<ruis::context> context;

	/**
	 * @brief Constructor.
	 * @param context - ruis context to use for this gui instance.
	 */
	gui(const utki::shared_ref<ruis::context>& context);

	gui(const gui&) = delete;
	gui& operator=(const gui&) = delete;

	gui(gui&&) = delete;
	gui& operator=(gui&&) = delete;

	~gui() = default;

private:
	utki::shared_ref<ruis::widget> root_widget;

public:
	/**
	 * @brief Set the root widget of the application.
	 * @param w - the widget to set as a root widget.
	 */
	void set_root(utki::shared_ref<ruis::widget> w);

	/**
	 * @brief Get current root widget.
	 * Root widget is always present, so it is safe to call this function at any time.
	 * By default, the root widget is set to a dummy ruis::widget object.
	 * @return reference to the currently set root widget.
	 */
	ruis::widget& get_root() noexcept
	{
		return this->root_widget.get();
	}

	/**
	 * @brief Get constant current root widget.
	 * Root widget is always present, so it is safe to call this function at any time.
	 * By default, the root widget is set to a dummy ruis::widget object.
	 * @return constant reference to the currently set root widget.
	 */
	const ruis::widget& get_root() const noexcept
	{
		return this->root_widget.get();
	}

private:
	vector2 viewport_size{0, 0};

public:
	/**
	 * @brief Set viewport size for GUI.
	 * Set the dimensions of the rectangular area where GUI will be rendered.
	 * @param size - dimensions of the viewport, in pixels.
	 */
	void set_viewport(const ruis::vector2& size);

	/**
	 * @brief Render GUI.
	 * Assumes that passed in matrix sets up a right-handed coordinate system so that:
	 * - x-axis right, left screen edge is at 0, right screen edge is at 1.
	 * - y-axis down, top screen edge is at 0, bottom screen edge is at 1.
	 * - z-axis away.
	 * @param matrix - use this transformation matrix. Usually a renderer's initial matrix.
	 */
	void render(const matrix4& matrix) const;

	/**
	 * @brief Initialize standard widgets library.
	 * In addition to core widgets it is possible to use standard widgets.
	 * This function loads necessary resource packs and initializes standard
	 * widgets to be used by application.
	 * @param fi - file interface to use for resource loading.
	 */
	void init_standard_widgets(papki::file& fi);

	/**
	 * @brief Update GUI.
	 * Call this function from main loop of the program.
	 * @return number of milliseconds to sleep before next call.
	 */
	uint32_t update()
	{
		return this->context.get().updater.get().update();
	}

	/**
	 * @brief Feed in the mouse move event to GUI.
	 * @param pos - new position of the mouse pointer.
	 * @param id - ID of the mouse pointer.
	 */
	void send_mouse_move(const vector2& pos, unsigned id);

	/**
	 * @brief Feed in the mouse button event to GUI.
	 * @param is_down - is mouse button pressed (true) or released (false).
	 * @param pos - position of the mouse pointer at the moment the button was pressed or released.
	 * @param button - mouse button.
	 * @param id - ID of the mouse pointer.
	 */
	void send_mouse_button(bool is_down, const vector2& pos, mouse_button button, unsigned id);

	/**
	 * @brief Feed in mouse hover event to GUI.
	 * Call this function when the mouse pointer enters or leaves the GUI viewport.
	 * @param is_hovered - whether the mouse pointer entered (true) the GUI area or left (false).
	 * @param id - mouse pointer ID.
	 */
	void send_mouse_hover(bool is_hovered, unsigned id);

private:
	utki::flags<ruis::key_modifier> key_modifiers;

public:
	/**
	 * @brief Feed in the key event to GUI.
	 * Note, this method is not supposed to receive repeated key events, when user holds down the key.
	 * @param is_down - is the key pressed (true) or released (false).
	 * @param key_code - code of the key.
	 */
	void send_key(bool is_down, key key_code);

	/**
	 * @brief Input string provider.
	 * Override this class to pass in the character input information when user makes character input.
	 */
	struct input_string_provider {
		/**
		 * @brief Get string.
		 * Override this function to return entered text.
		 * @return The text that the user has entered.
		 */
		virtual std::u32string get() const = 0;

		input_string_provider() = default;

		input_string_provider(const input_string_provider&) = default;
		input_string_provider& operator=(const input_string_provider&) = default;

		input_string_provider(input_string_provider&&) = default;
		input_string_provider& operator=(input_string_provider&&) = default;

		virtual ~input_string_provider() = default;
	};

	/**
	 * @brief Feed in the character input event to the GUI.
	 * The idea with input_string_provider parameter is that we don't want to calculate the unicode string
	 * unless it is really needed, thus provide the string only when get() method is called.
	 * This method is supposed to receive also a repeated key events when user holds down the key, as well as initial
	 * key press. input_string_provider may provide empty string.
	 * @param string_provider - input string provider.
	 * @param key_code - key code associated with character input, can be unknown.
	 */
	void send_character_input(const input_string_provider& string_provider, ruis::key key_code);
};

} // namespace ruis
