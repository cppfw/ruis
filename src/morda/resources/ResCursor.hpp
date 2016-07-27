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
class ResCursor : public Resource{
	std::shared_ptr<const ResImage> image_v;
	Vec2r hotspot_v;
	
public:
	ResCursor(ResImage& image, const Vec2r& hotspot);
	
	ResCursor(const ResCursor&) = delete;
	ResCursor& operator=(const ResCursor&) = delete;
	
	const ResImage& image()const noexcept{
		ASSERT(this->image_v)
		return *this->image_v;
	}
	
	const Vec2r& hotspot()const noexcept{
		return this->hotspot_v;
	}
private:
	friend class ResourceManager;
	
	static std::shared_ptr<ResCursor> load(const stob::Node& chain, const papki::File &fi);
};

}
