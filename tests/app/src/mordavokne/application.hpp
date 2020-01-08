#pragma once

#include <memory>

#include <utki/config.hpp>
#include <utki/singleton.hpp>
#include <utki/Buf.hpp>
#include <utki/Unique.hpp>
#include <utki/flags.hpp>

#include <papki/file.hpp>

#include <r4/vector2.hpp>

#include <morda/Morda.hpp>

#include <morda/util/key.hpp>

#include "config.hpp"



namespace mordavokne{

/**
 * @brief Desired window parameters.
 */
struct window_params{
	/**
	 * @brief Desired dimensions of the window
	 */
	r4::vec2ui dim;

	//TODO: add window title string

	enum class buffer_type{
		depth,
		stencil,

		ENUM_SIZE
	};

	/**
	 * @brief Flags describing desired buffers for OpneGL context.
	 */
	utki::flags<buffer_type> buffers = false;

	window_params(r4::vec2ui dim) :
			dim(dim)
	{}
};


/**
 * @brief Base singleton class of application.
 * An application should subclass this class and return an instance from the
 * application factory function create_application(), see application.hpp for details.
 * When instance of this class is created it also creates a window and
 * initializes rendering API (e.g. OpenGL or OpenGL ES).
 */
class application :
		public utki::intrusive_singleton<application>,
		public utki::Unique
{
	friend T_Singleton;
	static T_Instance instance;

public:
	const std::string name;

	// TODO: deprecated, remove.
	typedef window_params WindowParams;

private:
	std::unique_ptr<utki::Unique> windowPimpl;

	friend const decltype(windowPimpl)& getWindowPimpl(application& app);

private:
	void swapFrameBuffers();

public:
	morda::Morda gui;

public:

	/**
	 * @brief Create file interface into resources storage.
	 * This function creates a morda's standard file interface to read application's
	 * resources.
	 * @param path - file path to initialize the file interface with.
	 * @return Instance of the file interface into the resources storage.
	 */
	std::unique_ptr<papki::File> get_res_file(const std::string& path = std::string())const;

	std::unique_ptr<papki::File> getResFile(const std::string& path = std::string())const{
		return this->get_res_file(path);
	}

public:
	/**
	 * @brief Storage directory path.
	 * Path to the application's storage directory. This is the directory
	 * where application generated files are to be stored, like configurations,
	 * saved states, etc.
	 * The path is always ended with '/' character.
	 */
	const std::string storage_dir;

	// TODO: deprecated, remove.
	const std::string& storageDir = storage_dir;

private:
	//this is a viewport rectangle in coordinates that are as follows: x grows right, y grows up.
	morda::Rectr curWinRect = morda::Rectr(0, 0, 0, 0);

public:
	const morda::Vec2r& window_dimensions()const noexcept{
		return this->curWinRect.d;
	}

	// TODO: deprecated, remove.
	const morda::Vec2r& winDim()const noexcept{
		return this->window_dimensions();
	}

private:
	void render();

	friend void render(application& app);

	void updateWindowRect(const morda::Rectr& rect);

	friend void updateWindowRect(application& app, const morda::Rectr& rect);

	//pos is in usual window coordinates, y goes down.
	morda::Vec2r nativeWindowToRootCoordinates(const r4::vec2f& pos)const noexcept{
		return pos;
	}

	//pos is in usual window coordinates, y goes down.
	void handleMouseMove(const r4::vec2f& pos, unsigned id){
		this->gui.onMouseMove(this->nativeWindowToRootCoordinates(pos), id);
	}

	friend void handleMouseMove(application& app, const r4::vec2f& pos, unsigned id);

	//pos is in usual window coordinates, y goes down.
	void handleMouseButton(bool isDown, const r4::vec2f& pos, morda::MouseButton_e button, unsigned id){
		this->gui.onMouseButton(isDown, this->nativeWindowToRootCoordinates(pos), button, id);
	}

	friend void handleMouseButton(application& app, bool isDown, const r4::vec2f& pos, morda::MouseButton_e button, unsigned id);

	void handleMouseHover(bool isHovered, unsigned pointerID){
		this->gui.onMouseHover(isHovered, pointerID);
	}

	friend void handleMouseHover(application& app, bool isHovered, unsigned pointerID);

protected:
	/**
	 * @brief Application constructor.
	 * @param name - name of the application.
	 * @param requestedWindowParams - requested window parameters.
	 */
	application(std::string&& name, const window_params& requestedWindowParams);

public:

	virtual ~application()noexcept{}

	/**
	 * @brief Bring up the virtual keyboard.
	 * On mobile platforms this function will summon the on-screen keyboard.
	 * On desktop platforms this function does nothing.
	 */
	void show_virtual_keyboard()noexcept;

	/**
	 * @brief Hide virtual keyboard.
	 * On mobile platforms this function hides the on-screen keyboard.
	 * On desktop platforms this function does nothing.
	 */
	void hide_virtual_keyboard()noexcept;

private:

	//The idea with UnicodeResolver parameter is that we don't want to calculate the unicode unless it is really needed, thus postpone it
	//as much as possible.
	void handleCharacterInput(const morda::Morda::UnicodeProvider& unicodeResolver, morda::key key){
		this->gui.onCharacterInput(unicodeResolver, key);
	}

	friend void handleCharacterInput(application& app, const morda::Morda::UnicodeProvider& unicodeResolver, morda::key key);

	void handleKeyEvent(bool isDown, morda::key keyCode){
		this->gui.onKeyEvent(isDown, keyCode);
	}

	friend void handleKeyEvent(application& app, bool isDown, morda::key keyCode);

public:

	/**
	 * @brief Requests application to exit.
	 * This function posts an exit message to the applications message queue.
	 * The message will normally be handled on the next UI cycle and as a result
	 * the main loop will be terminated and application will exit. The Application
	 * object will be destroyed and all resources freed.
	 */
	void quit()noexcept;

private:
	bool isFullscreen_v = false;

	r4::rectu beforeFullScreenWindowRect;

public:
	/**
	 * @brief Check if application currently runs in fullscreen mode.
	 * @return true if application is in fullscreen mode.
	 * @return false if application is in windowed mode.
	 */
	bool is_fullscreen()const noexcept {
		return this->isFullscreen_v;
	}

	// TODO: deprecated, remove.
	bool isFullscreen()const noexcept {
		return this->is_fullscreen();
	}

	/**
	 * @brief Set/unset fullscreen mode.
	 * @param enable - whether to enable or to disable fullscreen mode.
	 */
	void set_fullscreen(bool enable);

	// TODO: deprecated, remove.
	void setFullscreen(bool enable){
		this->set_fullscreen(enable);
	}


	/**
	 * @brief Show/hide mouse cursor.
	 * @param visible - whether to show (true) or hide (false) mouse cursor.
	 */
	void set_mouse_cursor_visible(bool visible);

	// TODO: deprecated, remove.
	void setMouseCursorVisible(bool visible){
		this->set_mouse_cursor_visible(visible);
	}


	/**
	 * @brief Get dots per density pixel (dp) for given display parameters.
	 * The size of the dp for desktop displays should normally be equal to one pixel.
	 * For hand held devices size of the dp depends on physical screen size and pixel resolution.
	 * @param screen_size_pixels - resolution of the display in pixels.
	 * @param screen_size_mm - size of the display in millimeters.
	 * @return Size of one display density pixel in pixels.
	 */
	static morda::real get_pixels_per_dp(r4::vec2ui screen_size_pixels, r4::vec2ui screen_size_mm);

	// TODO: deprecated, remove.
	static morda::real findDotsPerDp(r4::vec2ui resolution, r4::vec2ui screenSizeMm){
		return get_pixels_per_dp(resolution, screenSizeMm);
	}
};

inline application& inst(){

	return application::inst();
}

// TODO: deprecaed, remove.
typedef application App;

/**
 * @brief Create application instance.
 * User needs to define this factory function to create his application instance.
 * @param argc - number of command line arguments, including the executable filename as 0th argument.
 * @param argv - array of command line arguments (0th argument is executable filename).
 * @return New application instance.
 */
#if M_OS == M_OS_WINDOWS
__declspec(dllexport)
#endif

std::unique_ptr<application> create_application(int argc, const char** argv);


//TODO: deprecated, remove createApp() function.
#if M_OS == M_OS_WINDOWS
__declspec(dllexport)
#endif
std::unique_ptr<application> createApp(int argc, const char** argv);



}
