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

#include <array>

#include <r4/rectangle.hpp>

#include "../context.hpp"
#include "../resource_loader.hpp"

#include "texture.hpp"

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
	image(const utki::shared_ref<ruis::context>& c);

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
	class texture
	{
	protected:
		const utki::shared_ref<const ruis::renderer> renderer;

		// NOLINTNEXTLINE(modernize-pass-by-value)
		texture(const utki::shared_ref<const ruis::renderer>& r, vector2 dims) :
			renderer(r),
			dims(dims)
		{}

	public:
		const vector2 dims;

		texture(const texture&) = delete;
		texture& operator=(const texture&) = delete;

		texture(texture&&) = delete;
		texture& operator=(texture&&) = delete;

		virtual ~texture() = default;

		void render(const matrix4& matrix) const
		{
			this->render(matrix, this->renderer.get().pos_tex_quad_01_vao.get());
		}

		/**
		 * @brief Render a quad with this texture.
		 * @param matrix - transformation matrix to use for rendering.
		 * @param vao - vertex array to use for rendering.
		 */
		virtual void render(const matrix4& matrix, const vertex_array& vao) const = 0;
	};

	/**
	 * @brief Get dimensions of this image in pixels.
	 * @return Dimensions of the image in pixels.
	 */
	virtual vector2 dims() const noexcept = 0;

	/**
	 * @brief Get raster texture of given dimensions.
	 * @param for_dims - dimensions request for raster texture.
	 *        If any of the dimensions is 0 then it will be adjusted to preserve aspect ratio.
	 *        If both dimensions are zero, then dimensions which are natural for the particular image will be used.
	 */
	virtual utki::shared_ref<const texture> get(vector2 for_dims = 0) const = 0;

private:
	static utki::shared_ref<image> load(
		const utki::shared_ref<ruis::context>& ctx,
		const ::treeml::forest& desc,
		const papki::file& fi
	);

public:
	/**
	 * @brief Load image resource from image file.
	 * Files supported are PNG, JPG, SVG.
	 * @param ctx - context.
	 * @param fi - image file.
	 * @return Loaded resource.
	 */
	static utki::shared_ref<image> load(const utki::shared_ref<ruis::context>& ctx, const papki::file& fi);
};

// TODO: is atlas_image needed?
/**
 * @brief Undocumented.
 */
class atlas_image : public image, public image::texture
{
	friend class image;

	const utki::shared_ref<const res::texture> tex;

	const utki::shared_ref<const vertex_array> vao;

public:
	// TODO:
	// atlas_image(
	// 	const utki::shared_ref<ruis::context>& c,
	// 	const utki::shared_ref<res::texture>& tex,
	// 	const rectangle& rect
	// );
	atlas_image(const utki::shared_ref<ruis::context>& c, const utki::shared_ref<res::texture>& tex);

	atlas_image(const atlas_image&) = delete;
	atlas_image& operator=(const atlas_image&) = delete;

	atlas_image(atlas_image&&) = delete;
	atlas_image& operator=(atlas_image&&) = delete;

	~atlas_image() override = default;

	vector2 dims() const noexcept override
	{
		return this->image::texture::dims;
	}

	utki::shared_ref<const image::texture> get(vector2 for_dims) const override;

	void render(const matrix4& matrix, const vertex_array& vao) const override;

private:
	static utki::shared_ref<atlas_image> load(
		const utki::shared_ref<ruis::context>& ctx,
		const ::treeml::forest& desc,
		const papki::file& fi
	);
};

} // namespace ruis::res
