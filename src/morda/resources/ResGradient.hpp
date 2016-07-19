#pragma once

#include <kolme/Vector2.hpp>

#include "../ResourceManager.hpp"
#include "../config.hpp"

#include "../shaders/ClrPosShader.hpp"


namespace morda{

/**
 * @brief Gradient resource.
 * 
 * %Resource description:
 * 
 * %List of stops.
 * 
 * Stop parameters:
 * 
 * @param pos - stop position from range [0:1].
 * @param color - stop color.
 * 
 * Example:
 * @code
 * morda_grd_window_background{
 *     stop{ pos{0} color{0} }
 *     stop{ pos{0.43} color{0x10ffffff} }
 *     stop{ pos{0.5} color{0x12ffffff} }
 *     stop{ pos{0.57} color{0x10ffffff} }
 *     stop{ pos{1} color{0} }
 * }
 * @endcode
 */
class ResGradient : public Resource{
	friend class ResourceManager;
	
	std::vector<kolme::Vec2f> vertices;
	std::vector<std::uint32_t> colors;
	
public:
	/**
	 * @brief Create gradient resource form aray of gradient stops.
	 * A gradient stop is a pair of values. First one is a floating point value
	 * from [0:1] defining the position of the gradient stop. The second value
	 * defines the color of the stop.
	 * @param stops - array of gradient stops.
	 * @param vertical - if true, the gradient is vertical. If false, the gradient is horizontal.
	 */
	ResGradient(const std::vector<std::tuple<real, std::uint32_t>>& stops, bool vertical);
	
	ResGradient(const ResGradient&) = delete;
	ResGradient& operator=(const ResGradient&) = delete;
	
	/**
	 * @brief render gradient.
	 * Renders the gradient as a rectangle ((0,0),(1,1)).
	 * @param matrix - transformation matrix to use when rendering.
	 * @param s - shader to use for rendering.
	 */
	//TODO: do not pass the matrix, it can be set to shader from outside.
	void render(const Matr4r& matrix, ClrPosShader& s)const;
	
private:
	static std::shared_ptr<ResGradient> load(const stob::Node& chain, const papki::File& fi);
};

}
