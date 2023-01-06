/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include <treeml/tree.hpp>

#include "../resource_loader.hpp"

#include "../render/texture_2d.hpp"

#include "../util/util.hpp"

namespace morda{ namespace res{

/**
 * @brief Texture resource.
 * 
 * %resource description:
 * 
 * @param file - name of the image file, can be raster image.
 * 
 * Example:
 * @code
 * tex_sample{
 *     file{texture_sample.png}
 * }
 * @endcode
 */
class texture : public morda::resource{
	friend class morda::resource_loader;

	const utki::shared_ref<texture_2d> tex2d;
public:
	/**
	 * @brief Create texture.
	 * @param c - context.
	 * @param tex - texture object to initialize this resource with.
	 */
	texture(
		const utki::shared_ref<morda::context>& c,
		const decltype(tex2d)& tex
	) :
			resource(c),
			tex2d(tex)
	{}

	~texture()noexcept{}

	/**
	 * @brief Get texture object held by this resource.
	 * @return Texture object.
	 */
	const texture_2d& tex()const noexcept{
		return this->tex2d.get();
	}

private:
	static utki::shared_ref<texture> load(
		const utki::shared_ref<morda::context>& ctx,
		const ::treeml::forest& desc,
		const papki::file& fi
	);
};

}}
