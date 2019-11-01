#pragma once

#include <r4/vector2.hpp>

#include "../ResourceManager.hpp"
#include "../config.hpp"

#include "../render/VertexArray.hpp"


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
 *     Stop{ pos{0} color{0} }
 *     Stop{ pos{0.43} color{0x10ffffff} }
 *     Stop{ pos{0.5} color{0x12ffffff} }
 *     Stop{ pos{0.57} color{0x10ffffff} }
 *     Stop{ pos{1} color{0} }
 * }
 * @endcode
 */
class ResGradient : public Resource{
	friend class ResourceManager;
	
	std::shared_ptr<VertexArray> vao;
	
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
	 * @param m - transformation matrix.
	 */
	void render(const morda::Matr4r& m)const;
	
private:
	static std::shared_ptr<ResGradient> load(const stob::Node& chain, const papki::File& fi);
};

}
