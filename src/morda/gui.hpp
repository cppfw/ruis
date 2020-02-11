#pragma once

#include <utki/singleton.hpp>

#include "context.hpp"

namespace morda{

class gui : public utki::intrusive_singleton<gui>{
	friend T_Singleton;
	static T_Instance instance;

	friend class Updateable;
	friend class widget;

public:
	const std::shared_ptr<morda::context> context;

	/**
	 * @brief Constructor.
	 * @param r - renderer implementation.
	 * @param post_to_ui_thread_function - function to use when posting an action to UI thread is needed.
	 * @param dots_per_inch - DPI of your display.
	 * @param dots_per_dp - desired dots per density pixel.
	 */
	gui(
			std::shared_ptr<morda::Renderer>&& r,
			std::function<void(std::function<void()>&&)>&& post_to_ui_thread_function,
			real dots_per_inch,
			real dots_per_dp
		);

	gui(const gui&) = delete;
	gui& operator=(const gui&) = delete;

	virtual ~gui()noexcept{}

private:
	//NOTE: this should go after resMan as it may hold references to some resources, so it should be destroyed first
	std::shared_ptr<morda::widget> rootWidget;

	std::weak_ptr<widget> focusedWidget;

	void setFocusedWidget(const std::shared_ptr<widget> w);

public:
	/**
	 * @brief Set the root widget of the application.
	 * @param w - the widget to set as a root widget.
	 */
	void setRootWidget(const std::shared_ptr<morda::widget> w);

private:
	Vec2r viewportSize;
public:
	/**
	 * @brief Set viewport size for GUI.
	 * Set the dimensions of the rectangular area where GUI will be rendered.
	 * @param size - dimensions of the viewport, in pixels.
	 */
	void setViewportSize(const morda::Vec2r& size);

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
		return this->context->updater.update();
	}

public:
	/**
	 * @brief Execute code on UI thread.
	 * This function should be thread-safe.
	 * This function should post a message/event to programs main loop event queue.
	 * When the event is handled it should execute the specified function.
	 * @param f - function to execute on UI thread.
	 */
	void postToUiThread(std::function<void()>&& f);

	/**
	 * @brief Feed in the mouse move event to GUI.
	 * @param pos - new position of the mouse pointer.
	 * @param id - ID of the mouse pointer.
	 */
	void onMouseMove(const Vec2r& pos, unsigned id);

	/**
	 * @brief Feed in the mouse button event to GUI.
	 * @param isDown - is mouse button pressed (true) or released (false).
	 * @param pos - position of the mouse pointer at the moment the button was pressed or released.
	 * @param button - mouse button.
	 * @param id - ID of the mouse pointer.
	 */
	void onMouseButton(bool isDown, const Vec2r& pos, MouseButton_e button, unsigned id);

	/**
	 * @brief Feed in mouse hover event to GUI.
	 * Call this function when the mouse pointer enters or leaves the GUI viewport.
	 * @param isHovered - whether the mouse pointer entered (true) the GUI area or left (false).
	 * @param id - mouse pointer ID.
	 */
	void onMouseHover(bool isHovered, unsigned id);

	/**
	 * @brief Feed in the key event to GUI.
	 * Note, this method is not supposed to receive repeated key events, when user holds down the key.
	 * @param isDown - is the key pressed (true) or released (false).
	 * @param keyCode - code of the key.
	 */
	void onKeyEvent(bool isDown, key keyCode);

	/**
	 * @brief Unicode input provider.
	 * Override this class to pass in the character input information when user makes character input.
	 */
	struct UnicodeProvider{
		/**
		 * @brief Get unicode string.
		 * Override this function to return entered text.
		 * @return The text that the user has entered.
		 */
		virtual std::u32string get()const = 0;

		virtual ~UnicodeProvider()noexcept{}
	};

	/**
	 * @brief Feed in the character input event to the GUI.
	 * The idea with UnicodeProvider parameter is that we don't want to calculate the unicode string
	 * unless it is really needed, thus provide the string only when get() method is called.
	 * This method is supposed to receive also a repeated key events when user holds down the key, as well as initial key press.
	 * UnicodeProvider may provide empty string.
	 * @param unicode - unicode string provider.
	 * @param key - key code associated with character input, can be UNKNOWN.
	 */
	void onCharacterInput(const UnicodeProvider& unicode, key key);
};


inline gui& inst(){
	return morda::gui::inst();
}

}
