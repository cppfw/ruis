/*
morda GUI framework
Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

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
		const std::array<std::array<std::shared_ptr<const res::image>, 3>, 3> img_matrix;
		
		std::weak_ptr<const nine_patch> parent;
	
		real mul;//for erasing from the cache
	public:
		decltype(img_matrix)& images()const noexcept{
			return this->img_matrix;
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
	
	static std::shared_ptr<nine_patch> load(morda::context& ctx, const ::treeml::forest& desc, const papki::file& fi);
};

}}
