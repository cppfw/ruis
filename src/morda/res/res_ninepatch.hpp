#pragma once

#include "../config.hpp"

#include "../resource_loader.hpp"

#include "ResImage.hpp"

namespace morda{


/**
 * @brief Nine-patch resource.
 * Nine-patch is a stretchable image with defined unstretchable borders.
 * 
 * %resource description:
 * 
 * @param file - name of the image file, can be raster image or SVG.
 * 
 * @param borders - widths of borders in pixels in the left-top-right-bottom order.
 * 
 * Example:
 * @code
 * morda_npt_rightbutton_pressed{
 *     file{rightbutton_pressed.svg}
 *     borders{5 5 5 6}
 * }
 * @endcode
 */
class res_ninepatch : public resource{
	friend class resource_loader;
	
	const std::shared_ptr<const ResImage> image;
	
	Sidesr borders_v;
	
public:
	res_ninepatch(const res_ninepatch&) = delete;
	res_ninepatch& operator=(const res_ninepatch&) = delete;
	
	res_ninepatch(std::shared_ptr<morda::context> c, std::shared_ptr<const ResImage> image, Sidesr borders) :
			resource(std::move(c)),
			image(std::move(image)),
			borders_v(borders)
	{}
	
	
	class image_matrix{
		const std::array<std::array<std::shared_ptr<const ResImage>, 3>, 3> images_v;
		
		std::weak_ptr<const res_ninepatch> parent;
	
		real mul;//for erasing from the cache
	public:
		decltype(images_v)& images()const noexcept{
			return this->images_v;
		}
		
		image_matrix(std::array<std::array<std::shared_ptr<const ResImage>, 3>, 3>&& l, std::shared_ptr<const res_ninepatch> parent, real mul);
		
		~image_matrix()noexcept;
	};
	
	std::shared_ptr<image_matrix> get(Sidesr borders)const;
	
	const decltype(borders_v)& borders()const noexcept{
		return this->borders_v;
	}
private:
	mutable std::map<real, std::weak_ptr<image_matrix>> cache;
	
	static std::shared_ptr<res_ninepatch> load(morda::context& ctx, const puu::forest& desc, const papki::file& fi);
};

}
