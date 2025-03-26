/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

namespace ruis::res {

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
 * ruis_npt_rightbutton_pressed{
 *     file{rightbutton_pressed.svg}
 *     borders{5 5 5 6}
 * }
 * @endcode
 */
class nine_patch : public resource
{
	friend class ruis::resource_loader;

public:
	const utki::shared_ref<ruis::render::renderer> renderer;

private:
	const utki::shared_ref<const res::image> image;

	sides<real> fraction_borders;

public:
	nine_patch(const nine_patch&) = delete;
	nine_patch& operator=(const nine_patch&) = delete;

	nine_patch(nine_patch&&) = delete;
	nine_patch& operator=(nine_patch&&) = delete;

	~nine_patch() override = default;

	nine_patch(
		utki::shared_ref<ruis::render::renderer> renderer,
		utki::shared_ref<const res::image> image,
		sides<real> fraction_borders
	);

	class image_matrix
	{
		const std::array<std::array<utki::shared_ref<const res::image>, 3>, 3> img_matrix;

		std::weak_ptr<const nine_patch> parent;

		real mul; // for erasing from the cache

	public:
		decltype(img_matrix)& images() const noexcept
		{
			return this->img_matrix;
		}

		image_matrix(
			std::array<std::array<utki::shared_ref<const res::image>, 3>, 3> l,
			std::weak_ptr<const nine_patch> parent,
			real mul
		);

		image_matrix(const image_matrix&) = delete;
		image_matrix& operator=(const image_matrix&) = delete;

		image_matrix(image_matrix&&) = delete;
		image_matrix& operator=(image_matrix&&) = delete;

		// NOLINTNEXTLINE(bugprone-exception-escape, "false positive")
		~image_matrix();
	};

	std::shared_ptr<image_matrix> get(
		const ruis::units& units, //
		sides<length> borders
	) const;

	sides<real> get_borders(const ruis::units& units) const noexcept;

private:
	mutable std::map<real, std::weak_ptr<image_matrix>> cache;

	static utki::shared_ref<nine_patch> load(
		const ruis::resource_loader& loader,
		const ::tml::forest& desc,
		const papki::file& fi
	);
};

} // namespace ruis::res
