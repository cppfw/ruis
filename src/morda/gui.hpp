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
	 * @param r - renderer implementation.
     * @param u - updater to use along with this gui.
	 * @param post_to_ui_thread_function - function to use when posting an action to UI thread is needed.
	 * @param dots_per_inch - DPI of your display.
	 * @param dots_per_dp - desired dots per density pixel.
	 */
	gui(
			std::shared_ptr<morda::renderer> r,
            std::shared_ptr<morda::updater> u,
			std::function<void(std::function<void()>&&)>&& post_to_ui_thread_function,
			real dots_per_inch,
			real dots_per_dp
		);

	gui(const gui&) = delete;
	gui& operator=(const gui&) = delete;

	virtual ~gui()noexcept{}

private:
	std::shared_ptr<morda::widget> rootWidget;

public:
	/**
	 * @brief Set the root widget of the application.
	 * @param w - the widget to set as a root widget.
	 */
	void set_root(std::shared_ptr<morda::widget> w);

private:
	Vec2r viewportSize;
public:
	/**
	 * @brief Set viewport size for GUI.
	 * Set the dimensions of the rectangular area where GUI will be rendered.
	 * @param size - dimensions of the viewport, in pixels.
	 */
	void set_viewport(const morda::Vec2r& size);

	/**
	 * @brief Render GUI.
	 * Y axis directed upwards. Left screen edge is at -1, right is at 1, top at 1, bottom at -1.
	 * @param matrix - use this transformation matrix.
	 */
	void render(const Matr4r& matrix = Matr4r().identity())const;

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
	std::uint32_t update(){
		return this->context->updater->update();
	}

	/**
	 * @brief Feed in the mouse move event to GUI.
	 * @param pos - new position of the mouse pointer.
	 * @param id - ID of the mouse pointer.
	 */
	void send_mouse_move(const Vec2r& pos, unsigned id);

	/**
	 * @brief Feed in the mouse button event to GUI.
	 * @param is_down - is mouse button pressed (true) or released (false).
	 * @param pos - position of the mouse pointer at the moment the button was pressed or released.
	 * @param button - mouse button.
	 * @param id - ID of the mouse pointer.
	 */
	void send_mouse_button(bool is_down, const Vec2r& pos, mouse_button button, unsigned id);

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
	 * The idea with UnicodeProvider parameter is that we don't want to calculate the unicode string
	 * unless it is really needed, thus provide the string only when get() method is called.
	 * This method is supposed to receive also a repeated key events when user holds down the key, as well as initial key press.
	 * UnicodeProvider may provide empty string.
	 * @param unicode - unicode string provider.
	 * @param key_code - key code associated with character input, can be unknown.
	 */
	void send_character_input(const unicode_provider& unicode, morda::key key_code);
};

}
