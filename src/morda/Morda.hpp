#pragma once

#include <utki/singleton.hpp>

#include "render/Renderer.hpp"

#include "util/mouse_button.hpp"

#include "Updateable.hpp"

#include "inflater.hpp"
#include "ResourceManager.hpp"

namespace morda{

class Morda : public utki::intrusive_singleton<Morda>{
	friend T_Singleton;
	static T_Instance instance;

	friend class Updateable;
	friend class widget;

	std::shared_ptr<Renderer> renderer_v;

public:

	Renderer& renderer()noexcept{
		ASSERT(this->renderer_v)
		return *this->renderer_v;
	}

	/**
	 * @brief Constructor.
	 * @param r - renderer implementation.
	 * @param dotsPerInch - DPI of your display.
	 * @param dotsPerDp - desired dots per density pixel.
	 * @param postToUiThreadFunction - function to use when posting an action to UI thread is needed.
	 */
	Morda(
			std::shared_ptr<morda::Renderer> r,
			real dotsPerInch,
			real dotsPerDp,
			std::function<void(std::function<void()>&&)>&& postToUiThreadFunction);

	Morda(const Morda&) = delete;
	Morda& operator=(const Morda&) = delete;

	virtual ~Morda()noexcept{}


private:
	Updateable::Updater updater;
public:

	/**
	 * @brief Instantiation of the resource manager.
	 */
	ResourceManager resMan;

	/**
	 * @brief Instantiation of the GUI inflater.
	 */
	morda::inflater inflater;


private:
	//NOTE: this should go after resMan as it may hold references to some resources, so it should be destroyed first
	std::shared_ptr<morda::Widget> rootWidget;

	std::weak_ptr<Widget> focusedWidget;

	void setFocusedWidget(const std::shared_ptr<Widget> w);

public:
	/**
	 * @brief Set the root widget of the application.
	 * @param w - the widget to set as a root widget.
	 */
	void setRootWidget(const std::shared_ptr<morda::Widget> w);

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
	void initStandardWidgets(papki::File& fi);


	/**
	 * @brief Update GUI.
	 * Call this function from main loop of the program.
	 * @return number of milliseconds to sleep before next call.
	 */
	std::uint32_t update(){
		return this->updater.update();
	}

private:
	std::function<void(std::function<void()>&&)> postToUiThread_v;
public:
	/**
	 * @brief Execute code on UI thread.
	 * This function should be thread-safe.
	 * This function should post a message/event to programs main loop event queue.
	 * When the event is handled it should execute the specified function.
	 * @param f - function to execute on UI thread.
	 */
	void postToUiThread(std::function<void()>&& f);

	//TODO: remove deprecated function
	void postToUiThread_ts(std::function<void()>&& f){
		TRACE(<< "postToUiThread_ts(): DEPRECATED! use postToUiThread() instead" << std::endl)
		this->postToUiThread(std::move(f));
	}

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


	/**
	 * @brief Information about screen units.
	 * This class holds information about screen units and performs conversion
	 * from one unit to another.
	 * In morda, length can be expressed in pixels, millimeters or points.
	 * Points is a convenience unit which is different depending on the screen dimensions
	 * and density. Point is never less than one pixel.
	 * For normal desktop displays like HP or Full HD point is equal to one pixel.
	 * For higher density desktop displays point is more than one pixel depending on density.
	 * For mobile platforms the point is also 1 or more pixels depending on display density and physical size.
	 */
	class Units{
		real dotsPerInch_v;
		real dotsPerDp_v;
	public:
		/**
		 * @brief Constructor.
		 * @param dotsPerInch - dots per inch.
		 * @param dotsPerDp - dots per density pixel.
		 */
		Units(real dotsPerInch, real dotsPerDp) :
				dotsPerInch_v(dotsPerInch),
				dotsPerDp_v(dotsPerDp)
		{}

		/**
		 * @brief Get dots (pixels) per inch.
		 * @return Dots per inch.
		 */
		real dpi()const noexcept{
			return this->dotsPerInch_v;
		}

		/**
		 * @brief Get dots (pixels) per centimeter.
		 * @return Dots per centimeter.
		 */
		real dotsPerCm()const noexcept{
			return this->dpi() / 2.54f;
		}

		/**
		 * @brief Get dots (pixels) per point.
		 * @return Dots per point.
		 */
		real dotsPerDp()const noexcept{
			return this->dotsPerDp_v;
		}

		/**
		 * @brief Convert millimeters to pixels (dots).
		 * @param mm - value in millimeters.
		 * @return Value in pixels.
		 */
		real mmToPx(real mm)const noexcept{
			return std::round(mm * this->dotsPerCm() / 10.0f);
		}

		/**
		 * @brief Convert points to pixels.
		 * @param dp - value in density pixels.
		 * @return  Value in pixels.
		 */
		real dpToPx(real dp)const noexcept{
			return std::round(dp * this->dotsPerDp());
		}
	} units;
};


inline Morda& inst(){
	return morda::Morda::inst();
}

}
