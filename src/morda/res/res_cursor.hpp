#pragma once

#include "ResImage.hpp"

namespace morda{

/**
 * @brief Mouse cursor resource.
 * 
 * @param image - reference to image resource.
 * @param hotspot - X and Y of the hot spot from top left corner of the image.
 * 
 * Example:
 * @code
 * crs_arrow{
 *     image{img_arrow}
 *     hotspot{2 3}
 * }
 * @endcode
 */
class res_cursor : public resource{
	std::shared_ptr<const ResImage> image_v;
	Vec2r hotspot_v;
	
public:
	res_cursor(std::shared_ptr<morda::context> c, ResImage& image, const Vec2r& hotspot);
	
	res_cursor(const res_cursor&) = delete;
	res_cursor& operator=(const res_cursor&) = delete;
	
	const ResImage& image()const noexcept{
		ASSERT(this->image_v)
		return *this->image_v;
	}
	
	const Vec2r& hotspot()const noexcept{
		return this->hotspot_v;
	}
private:
	friend class resource_loader;
	
	static std::shared_ptr<res_cursor> load(morda::context& ctx, const puu::forest& desc, const papki::file &fi);
};

}
