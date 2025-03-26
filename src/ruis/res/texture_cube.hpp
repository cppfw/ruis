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

#include "../render/texture_cube.hpp"
#include "../resource_loader.hpp"

namespace ruis::res {

/**
 * @brief Cube map texture resource.
 *
 * %resource description:
 *
 * @param file_px - name of the image file for positive-x cube face, can be raster image.
 * @param file_nx - name of the image file for negative-x cube face, can be raster image.
 * @param file_py - name of the image file for positive-y cube face, can be raster image.
 * @param file_ny - name of the image file for negative-y cube face, can be raster image.
 * @param file_pz - name of the image file for positive-z cube face, can be raster image.
 * @param file_nz - name of the image file for negative-z cube face, can be raster image.
 *
 * TODO:
 * @param min_filter - texture minification filter, possible values are [nearest, linear]. Default is 'linear'.
 * @param mag_filter - texture magnification filter, possible values are [nearest, linear]. Default is 'linear'.
 * @param mipmap - texture mipmapping, possible values are [none, nearest, linear]. Default is 'nearest'.
 *
 * Example:
 * @code
 * cube_sample{
 *     file_px{cube_sample_px.png}
 *     file_nx{cube_sample_nx.png}
 *     file_py{cube_sample_py.png}
 *     file_ny{cube_sample_ny.png}
 *     file_pz{cube_sample_pz.png}
 *     file_nz{cube_sample_nz.png}
 *
 *     // TODO:
 *     min_filter{linear}
 *     mag_filter{linear}
 *     mipmap{nearest}
 * }
 * @endcode
 */
class texture_cube : public ruis::resource
{
	friend class ruis::resource_loader;

	const utki::shared_ref<const render::texture_cube> tex_cube;

public:
	texture_cube(utki::shared_ref<const render::texture_cube> tex);

	texture_cube(const texture_cube&) = delete;
	texture_cube& operator=(const texture_cube&) = delete;

	texture_cube(texture_cube&&) = delete;
	texture_cube& operator=(texture_cube&&) = delete;

	~texture_cube() override = default;

	/**
	 * @brief Get cube map texture object held by this resource.
	 * @return Cube map texture object.
	 */
	const render::texture_cube& tex() const noexcept
	{
		return this->tex_cube.get();
	}

private:
	static utki::shared_ref<texture_cube> load( //
		const ruis::resource_loader& loader,
		const ::tml::forest& desc,
		const papki::file& fi
	);
};

} // namespace ruis::res
