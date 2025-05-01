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

#include <array>

#include <r4/rectangle.hpp>
#include <rasterimage/dimensioned.hpp>

#include "../context.hpp"
#include "../resource_loader.hpp"

#include "texture_2d.hpp"

namespace ruis::res {

/**
 * @brief Base image resource class.
 *
 * %resource description:
 *
 * @param file - name of the file to read the image from, can be raster image or SVG.
 *
 * Example:
 * @code
 * img_dropdown_arrow{
 *     file{dropdown_arrow.svg}
 * }
 * @endcode
 */
class image : public resource
{
	friend class ruis::resource_loader;

protected:
	image() = default;

public:
	image(const image&) = delete;
	image& operator=(const image&) = delete;

	image(image&&) = delete;
	image& operator=(image&&) = delete;

	~image() override = default;

	/**
	 * @brief Texture created from the image resource.
	 * The texture which was created from an image resource.
	 * This texture is to be used on a quad to render the image.
	 */
	class texture : public rasterimage::dimensioned
	{
	protected:
		const utki::shared_ref<const ruis::render::renderer> renderer;

		const utki::shared_ref<const render::texture_2d> tex_2d;

	public:
		// NOLINTNEXTLINE(modernize-pass-by-value)
		texture(
			utki::shared_ref<const ruis::render::renderer> r, //
			utki::shared_ref<const render::texture_2d> tex_2d
		) :
			rasterimage::dimensioned(tex_2d.get().dims()),
			renderer(std::move(r)),
			tex_2d(std::move(tex_2d))
		{}

		texture(const texture&) = delete;
		texture& operator=(const texture&) = delete;

		texture(texture&&) = delete;
		texture& operator=(texture&&) = delete;

		virtual ~texture() = default;

		void render(const matrix4& matrix) const
		{
			this->render(
				matrix, //
				this->renderer.get().obj().pos_tex_quad_01_vao.get()
			);
		}

		/**
		 * @brief Render a VAO with this texture.
		 * @param matrix - transformation matrix to use for rendering.
		 * @param vao - vertex array to use for rendering.
		 */
		virtual void render(
			const matrix4& matrix, //
			const render::vertex_array& vao
		) const
		{
			this->renderer.get().shaders().pos_tex->render(
				matrix, //
				vao,
				this->tex_2d.get()
			);
		}
	};

	/**
	 * @brief Get dimensions of this image in pixels.
	 * @param units - units to use for dimensions calculation. Used for scalable vector images.
	 * @return Dimensions of the image in pixels.
	 */
	virtual r4::vector2<uint32_t> dims(const ruis::units& units) const noexcept = 0;

	/**
	 * @brief Get raster texture of given dimensions.
	 * @param units - units to use for dimensions calculation. Used for scalable vector images.
	 * @param for_dims - dimensions request for raster texture.
	 *        If any of the dimensions is 0 then it will be adjusted to preserve aspect ratio.
	 *        If both dimensions are zero, then dimensions which are natural for the particular image will be used.
	 */
	virtual utki::shared_ref<const texture> get(
		const ruis::units& units, //
		vector2 for_dims = 0
	) const = 0;

private:
	static utki::shared_ref<image> load(
		const ruis::resource_loader& loader,
		const ::tml::forest& desc,
		const papki::file& fi
	);

public:
	/**
	 * @brief Load image resource from image file.
	 * Files supported are PNG, JPG, SVG.
	 * @param loader - resource loader.
	 * @param fi - image file.
	 * @return Loaded resource.
	 */
	static utki::shared_ref<image> load( //
		const ruis::resource_loader& loader,
		const papki::file& fi
	);
};

} // namespace ruis::res
