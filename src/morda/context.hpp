#pragma once

#include "render/Renderer.hpp"

#include "util/mouse_button.hpp"

#include "updateable.hpp"

#include "inflater.hpp"
#include "resource_loader.hpp"

namespace morda{

class context : public std::enable_shared_from_this<context>{
	friend class widget;
	friend class gui;

	std::weak_ptr<widget> focused_widget;

	void set_focused_widget(std::shared_ptr<widget> w);

public:
	const std::shared_ptr<Renderer> renderer;

	const std::shared_ptr<morda::updater> updater;

	const std::function<void(std::function<void()>&&)> run_from_ui_thread;

	/**
	 * @brief Instantiation of the resource loader.
	 */
	resource_loader loader;

	/**
	 * @brief Instantiation of the GUI inflater.
	 */
	morda::inflater inflater;

	/**
	 * @brief Constructor.
	 * @param r - renderer implementation.
	 * @param u - updater to use along with this context.
	 * @param run_from_ui_thread_function - function to use when posting an action to UI thread is needed.
	 * @param dots_per_inch - DPI of your display.
	 * @param dots_per_dp - desired dots per density pixel.
	 */
	context(
			std::shared_ptr<morda::Renderer>&& r,
			std::shared_ptr<morda::updater>&& u,
			std::function<void(std::function<void()>&&)>&& run_from_ui_thread_function,
			real dots_per_inch,
			real dots_per_dp
		);
	
	context(const context&) = delete;
	context(context&&) = delete;
	context& operator=(const context&) = delete;

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
	class units_info{
	public:
		const real dots_per_inch;

		/**
		 * @brief Dots per density pixel.
		 */
		const real dots_per_dp;

		/**
		 * @brief Constructor.
		 * @param dotsPerInch - dots per inch.
		 * @param dotsPerDp - dots per density pixel.
		 */
		units_info(real dots_per_inch, real dots_per_dp) :
				dots_per_inch(dots_per_inch),
				dots_per_dp(dots_per_dp)
		{}

		/**
		 * @brief Get dots (pixels) per centimeter.
		 * @return Dots per centimeter.
		 */
		real dots_per_cm()const noexcept{
			return this->dots_per_inch / real(2.54f);
		}

		/**
		 * @brief Convert millimeters to pixels (dots).
		 * @param mm - value in millimeters.
		 * @return Value in pixels.
		 */
		real mm_to_px(real mm)const noexcept{
			return std::round(mm * this->dots_per_cm() / real(10.0f));
		}

		/**
		 * @brief Convert density pixels to pixels.
		 * @param dp - value in density pixels.
		 * @return  Value in pixels.
		 */
		real dp_to_px(real dp)const noexcept{
			return std::round(dp * this->dots_per_dp);
		}
	} units;
};

}
