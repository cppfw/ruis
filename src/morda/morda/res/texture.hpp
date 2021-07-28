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

	std::shared_ptr<texture_2d> tex_v;
public:
	/**
	 * @brief Create texture.
	 * @param c - context.
	 * @param texture - texture object to initialize this resource with.
	 */
	texture(std::shared_ptr<morda::context> c, decltype(tex_v) texture) :
			resource(std::move(c)),
			tex_v(std::move(texture))
	{
		ASSERT(this->tex_v)
	}

	~texture()noexcept{}

	/**
	 * @brief Get texture object held by this resource.
	 * @return Texture object.
	 */
	const texture_2d& tex()const noexcept{
		return *this->tex_v;
	}

private:
	static std::shared_ptr<texture> load(morda::context& ctx, const ::treeml::forest& desc, const papki::file& fi);
};

}}
