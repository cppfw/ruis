#pragma once

#include "../config.hpp"

#include "../resource_loader.hpp"

#include "image.hpp"

namespace morda{ namespace res{

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
class nine_patch : public resource{
	friend class morda::resource_loader;
	
	const std::shared_ptr<const res::image> image;
	
	sides<real> borders_v;
	
public:
	nine_patch(const nine_patch&) = delete;
	nine_patch& operator=(const nine_patch&) = delete;
	
	nine_patch(std::shared_ptr<morda::context> c, std::shared_ptr<const res::image> image, sides<real> borders) :
			resource(std::move(c)),
			image(std::move(image)),
			borders_v(borders)
	{}
	
	
	class image_matrix{
		const std::array<std::array<std::shared_ptr<const res::image>, 3>, 3> images_v;
		
		std::weak_ptr<const nine_patch> parent;
	
		real mul;//for erasing from the cache
	public:
		decltype(images_v)& images()const noexcept{
			return this->images_v;
		}
		
		image_matrix(std::array<std::array<std::shared_ptr<const res::image>, 3>, 3>&& l, std::shared_ptr<const nine_patch> parent, real mul);
		
		~image_matrix()noexcept;
	};
	
	std::shared_ptr<image_matrix> get(sides<real> borders)const;
	
	const decltype(borders_v)& borders()const noexcept{
		return this->borders_v;
	}
private:
	mutable std::map<real, std::weak_ptr<image_matrix>> cache;
	
	static std::shared_ptr<nine_patch> load(morda::context& ctx, const ::puu::forest& desc, const papki::file& fi);
};

}}
