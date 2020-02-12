#pragma once

#include "../config.hpp"

namespace morda{
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
class units{
public:
    const real dots_per_inch;

    /**
     * @brief Dots per density pixel.
     */
    const real dots_per_dp;

    /**
     * @brief Constructor.
     * @param dots_per_inch - dots per inch.
     * @param dots_per_dp - dots per density pixel.
     */
    units(real dots_per_inch, real dots_per_dp) :
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
};

}
