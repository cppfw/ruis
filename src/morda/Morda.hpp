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

#include "widgets/core/CharInputWidget.hpp"//TODO: move to cpp

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
	//TODO: doxygen
	void setViewportSize(const morda::Vec2r& size);
	
	//TODO: doxygen
	void render()const;
	
	/**
	 * @brief Initialize standard widgets library.
	 * In addition to core widgets it is possible to use standard widgets.
	 * This function loads necessarey resource packs and initializes standard
	 * widgets to be used by application.
	 */
	void initStandardWidgets();
	
	
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
	
	//TODO: doxygen
	//pos is in usual window coordinates, y goes down.
	void handleMouseMove(const kolme::Vec2f& pos, unsigned id);

	//TODO: doxygen
	//pos is in usual window coordinates, y goes down.
	void handleMouseButton(bool isDown, const kolme::Vec2f& pos, Widget::MouseButton_e button, unsigned id);

	//TODO: doxygen
	void handleMouseHover(bool isHovered, unsigned pointerID);
	
	//TODO: doxygen
	void handleKeyEvent(bool isDown, Key_e keyCode);
	
	struct UnicodeProvider{
		virtual std::u32string get()const = 0;
		
		virtual ~UnicodeProvider()noexcept{}
	};
	
	//TODO: doxygen
	//The idea with UnicodeResolver parameter is that we don't want to calculate the unicode unless it is really needed, thus postpone it
	//as much as possible.
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
