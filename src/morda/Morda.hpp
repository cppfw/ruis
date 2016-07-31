#pragma once

#include <utki/Singleton.hpp>

#include "shaders/ColorPosShader.hpp"
#include "shaders/ColorPosTexShader.hpp"
#include "shaders/ClrPosShader.hpp"
#include "shaders/PosTexShader.hpp"
#include "shaders/SimpleGrayscalePosTexShader.hpp"
#include "shaders/SimpleBlurPosTexShader.hpp"

#include "Updateable.hpp"

#include "Inflater.hpp"
#include "ResourceManager.hpp"

namespace morda{

class Morda : public utki::IntrusiveSingleton<Morda>{
	friend T_Singleton;
	static T_Instance instance;
	
	friend class Updateable;
	friend class Widget;
public:

	/**
	 * @brief Constructor.
	 * @param dotsPerInch - dpi of your display.
	 * @param dotsPerPt - desired dots per point.
	 */
	Morda(real dotsPerInch, real dotsPerPt) :
			units(dotsPerInch, dotsPerPt)
	{}

	Morda(const Morda&) = delete;
	Morda& operator=(const Morda&) = delete;
	
	virtual ~Morda()noexcept{}

	
	/**
	 * @brief Collection of standard shaders.
	 */
	struct DefaultShaders{
		ColorPosShader colorPosShader;
		ColorPosTexShader colorPosTexShader;
		ClrPosShader clrPosShader;
		PosTexShader posTexShader;
		SimpleGrayscalePosTexShader simpleGrayscalePosTexShader;
		SimpleBlurPosTexShader simpleBlurPosTexShader;
	} shaders;

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
	Inflater inflater;
	
	
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
	void setRootWidget(const std::shared_ptr<morda::Widget>& w);
	
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
	 * @param matrix - use this transformation matrix.
	 */
	void render(const Matr4r& matrix = Matr4r().identity())const;
	
	/**
	 * @brief Initialize standard widgets library.
	 * In addition to core widgets it is possible to use standard widgets.
	 * This function loads necessarey resource packs and initializes standard
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
	
	/**
	 * @brief Execute code on UI thread.
	 * This function should be thread-safe.
	 * This function should post a message/event to programs main loop event queue.
	 * When the event is handled it should execute the specified function.
	 * @param f - function to execute on UI thread.
	 */
	virtual void postToUiThread_ts(std::function<void()>&& f) = 0;
	
	/**
	 * @brief Feed in the mouse move event to GUI.
	 * @param pos - new position of the mouse pointer.
	 * @param id - ID of the mouse pointer.
	 */
	void handleMouseMove(const kolme::Vec2f& pos, unsigned id);

	/**
	 * @brief Feed in the mouse button event to GUI.
	 * @param isDown - is mouse button pressed (true) or released (false).
	 * @param pos - position of the mouse pointer at the moment the button was pressed or released.
	 * @param button - mouse button.
	 * @param id - ID of the mouse pointer.
	 */
	void handleMouseButton(bool isDown, const kolme::Vec2f& pos, Widget::MouseButton_e button, unsigned id);

	/**
	 * @brief Feed in mouse hover event to GUI.
	 * Call this function when the mouse pointer enters or leaves the GUI viewport.
	 * @param isHovered - whether the mouse pointer entered (true) the GUI area or left (false).
	 * @param id - mouse pointer ID.
	 */
	void handleMouseHover(bool isHovered, unsigned id);
	
	/**
	 * @brief Feed in the key event to GUI.
	 * @param isDown - is the key pressed (true) or released (false).
	 * @param keyCode - code of the key.
	 */
	void handleKeyEvent(bool isDown, Key_e keyCode);
	
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
	 * @param unicode - unicode string provider.
	 * @param key - key code associated with character input, can be UNKNOWN.
	 */
	void handleCharacterInput(const UnicodeProvider& unicode, Key_e key);
	
	
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
		real dotsPerPt_v;
	public:
		/**
		 * @brief Constructor.
		 * @param dotsPerInch - dots per inch.
		 * @param dotsPerPt - dots per point.
		 */
		Units(real dotsPerInch, real dotsPerPt) :
				dotsPerInch_v(dotsPerInch),
				dotsPerPt_v(dotsPerPt)
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
		real dotsPerPt()const noexcept{
			return this->dotsPerPt_v;
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
		 * @param pt - value in points.
		 * @return  Value in pixels.
		 */
		real ptToPx(real pt)const noexcept{
			return std::round(pt * this->dotsPerPt());
		}
	} units;
};

}
