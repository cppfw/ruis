/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include <tml/tree.hpp>

#include "../render/texture_2d.hpp"
#include "../resource_loader.hpp"
#include "../util/util.hpp"

namespace ruis::res {

/**
 * @brief Texture resource.
 *
 * %resource description:
 *
 * @param file - name of the image file, can be raster image.
 * @param min_filter - texture minification filter, possible values are [nearest, linear]. Default is 'linear'.
 * @param mag_filter - texture magnification filter, possible values are [nearest, linear]. Default is 'linear'.
 * @param mipmap - texture mipmapping, possible values are [none, nearest, linear]. Default is 'nearest'.
 *
 * Example:
 * @code
 * tex_sample{
 *     file{texture_sample.png}
 *     min_filter{linear}
 *     mag_filter{linear}
 *     mipmap{nearest}
 * }
 * @endcode
 */
class texture_2d : public ruis::resource
{
	friend class ruis::resource_loader;

	const utki::shared_ref<const render::texture_2d> tex2d;

public:
	/**
	 * @brief Create 2d texture resource.
	 * @param c - ruis context.
	 * @param tex - 2d texture object to initialize this resource with.
	 */
	texture_2d( //
		utki::shared_ref<ruis::context> c,
		utki::shared_ref<const render::texture_2d> tex
	);

	texture_2d(const texture_2d&) = delete;
	texture_2d& operator=(const texture_2d&) = delete;

	texture_2d(texture_2d&&) = delete;
	texture_2d& operator=(texture_2d&&) = delete;

	~texture_2d() override = default;

	/**
	 * @brief Get 2d texture object held by this resource.
	 * @return 2d texture object.
	 */
	const render::texture_2d& tex() const noexcept
	{
		return this->tex2d.get();
	}

private:
	static utki::shared_ref<texture_2d> load( //
		utki::shared_ref<ruis::context> ctx,
		const ::tml::forest& desc,
		const papki::file& fi
	);
};

} // namespace ruis::res
