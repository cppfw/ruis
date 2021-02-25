#pragma once

#include <r4/vector.hpp>

#include <treeml/tree.hpp>

#include "../resource_loader.hpp"
#include "../config.hpp"

#include "../render/vertex_array.hpp"

#include "../util/util.hpp"

namespace morda{ namespace res{

/**
 * @brief Gradient resource.
 * 
 * %resource description:
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
class gradient : public resource{
	friend class morda::resource_loader;

	std::shared_ptr<vertex_array> vao;
	
public:
	/**
	 * @brief Create gradient resource form aray of gradient stops.
	 * A gradient stop is a pair of values. First one is a floating point value
	 * from [0:1] defining the position of the gradient stop. The second value
	 * defines the color of the stop.
	 * @param c - context.
	 * @param stops - array of gradient stops.
	 * @param vertical - if true, the gradient is vertical. If false, the gradient is horizontal.
	 */
	gradient(std::shared_ptr<morda::context> c, std::vector<std::tuple<real, uint32_t>>& stops, bool vertical);
	
	gradient(const gradient&) = delete;
	gradient& operator=(const gradient&) = delete;
	
	/**
	 * @brief render gradient.
	 * Renders the gradient as a rectangle ((0,0),(1,1)).
	 * @param m - transformation matrix.
	 */
	void render(const morda::matrix4& m)const;
	
private:
	static std::shared_ptr<gradient> load(morda::context& ctx, const ::treeml::forest& desc, const papki::file& fi);
};

}}
