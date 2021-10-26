/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

namespace morda{

class gui{
	friend class updateable;
	friend class widget;

public:
	const std::shared_ptr<morda::context> context;

	/**
	 * @brief Constructor.
	 * @param context - morda context to use for this gui instance.
	 */
	gui(std::shared_ptr<morda::context> context);

	gui(const gui&) = delete;
	gui& operator=(const gui&) = delete;

	virtual ~gui()noexcept{}

private:
	std::shared_ptr<morda::widget> root_widget;

public:
	/**
	 * @brief Set the root widget of the application.
	 * @param w - the widget to set as a root widget.
	 */
	void set_root(std::shared_ptr<morda::widget> w);

	/**
	 * @brief Get current root widget.
	 * @return pointer to the currently set root widget.
	 * @return nullptr if there is no root widget set.
	 */
	morda::widget* get_root()noexcept{
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
	void set_viewport(const morda::vector2& size);

	/**
	 * @brief Render GUI.
	 * Y axis directed upwards. Left screen edge is at -1, right is at 1, top at 1, bottom at -1.
	 * @param matrix - use this transformation matrix.
	 */
	void render(const matrix4& matrix = matrix4().set_identity())const;

	/**
	 * @brief Initialize standard widgets library.
	 * In addition to core widgets it is possible to use standard widgets.
	 * This function loads necessary resource packs and initializes standard
	 * widgets to be used by application.
	 * @param fi - file interface to use for resource loading.
	 */
	void initStandardWidgets(papki::file& fi);


	/**
	 * @brief Update GUI.
	 * Call this function from main loop of the program.
	 * @return number of milliseconds to sleep before next call.
	 */
	uint32_t update(){
		return this->context->updater->update();
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

	/**
	 * @brief Feed in the key event to GUI.
	 * Note, this method is not supposed to receive repeated key events, when user holds down the key.
	 * @param is_down - is the key pressed (true) or released (false).
	 * @param key_code - code of the key.
	 */
	void send_key(bool is_down, key key_code);

	/**
	 * @brief Unicode input provider.
	 * Override this class to pass in the character input information when user makes character input.
	 */
	struct unicode_provider{
		/**
		 * @brief Get unicode string.
		 * Override this function to return entered text.
		 * @return The text that the user has entered.
		 */
		virtual std::u32string get()const = 0;

		virtual ~unicode_provider()noexcept{}
	};

	/**
	 * @brief Feed in the character input event to the GUI.
	 * The idea with unicode_provider parameter is that we don't want to calculate the unicode string
	 * unless it is really needed, thus provide the string only when get() method is called.
	 * This method is supposed to receive also a repeated key events when user holds down the key, as well as initial key press.
	 * unicode_provider may provide empty string.
	 * @param unicode - unicode string provider.
	 * @param key_code - key code associated with character input, can be unknown.
	 */
	void send_character_input(const unicode_provider& unicode, morda::key key_code);
};

}
