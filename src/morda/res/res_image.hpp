#pragma once

#include <array>

#include <r4/rectangle.hpp>

#include "res_texture.hpp"

#include "../resource_loader.hpp"

#include "../context.hpp"

namespace morda{

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
class res_image : public resource{
	friend class resource_loader;
	
protected:
	res_image(std::shared_ptr<morda::context> c);
	
public:
	res_image(const res_image& orig) = delete;
	res_image& operator=(const res_image& orig) = delete;
	
	/**
	 * @brief Texture created from the image resource.
	 * The texture which was created from an image resource.
	 * This texture is to be used on a quad to render the image.
	 */
	class texture{
	protected:
		const std::shared_ptr<morda::renderer> renderer;

		texture(std::shared_ptr<morda::renderer> r, Vec2r dims) :
				renderer(std::move(r)),
				dims(dims)
		{}
	public:
		const Vec2r dims;
		
		virtual ~texture()noexcept{}

		void render(const matrix4& matrix)const{
			this->render(matrix, *this->renderer->pos_tex_quad_01_vao);
		}

		/**
		 * @brief Render a quad with this texture.
		 * @param matrix - transformation matrix to use for rendering.
		 * @param vao - vertex array to use for rendering.
		 */
		virtual void render(const matrix4& matrix, const vertex_array& vao)const = 0;
	};

	/**
	 * @brief Get dimensions of this image in pixels for given Dots Per Inch resolution.
	 * @param dpi - dots per inch.
	 * @return Dimensions of the image in pixels.
	 */
	virtual Vec2r dims(real dpi)const noexcept = 0;
	
	Vec2r dims()const noexcept{
		return this->dims(this->context->units.dots_per_inch);
	}

	/**
	 * @brief Get raster texture of given dimensions.
	 * @param forDims - dimensions request for raster texture.
	 *        If any of the dimensions is 0 then it will be adjusted to preserve aspect ratio.
	 *        If both dimensions are zero, then dimensions which are natural for the particular image will be used.
	 */
	virtual std::shared_ptr<const texture> get(Vec2r forDims = 0)const = 0;
private:
	static std::shared_ptr<res_image> load(morda::context& ctx, const puu::forest& desc, const papki::file& fi);
	
public:
	/**
	 * @brief Load image resource from image file.
	 * Files supported are PNG, JPG, SVG.
	 * @param ctx - context.
	 * @param fi - image file.
	 * @return Loaded resource.
	 */
	static std::shared_ptr<res_image> load(morda::context& ctx, const papki::file& fi);
};


/**
 * @brief Undocumented.
 */
class res_atlas_image : public res_image, public res_image::texture{
	friend class res_image;
	
	std::shared_ptr<res_texture> tex;
	
	std::shared_ptr<vertex_array> vao;
	
public:
	res_atlas_image(std::shared_ptr<morda::context> c, std::shared_ptr<res_texture> tex, const rectangle& rect);
	res_atlas_image(std::shared_ptr<morda::context> c, std::shared_ptr<res_texture> tex);
	
	res_atlas_image(const res_atlas_image& orig) = delete;
	res_atlas_image& operator=(const res_atlas_image& orig) = delete;
	
	Vec2r dims(real dpi) const noexcept override{
		return this->res_image::texture::dims;
	}
	
	virtual std::shared_ptr<const res_image::texture> get(Vec2r forDim)const override{
		return std::dynamic_pointer_cast<const res_image::texture>(this->shared_from_this());
	}
	
	void render(const matrix4& matrix, const vertex_array& vao) const override;
	
private:
	static std::shared_ptr<res_atlas_image> load(morda::context& ctx, const puu::forest& desc, const papki::file& fi);
};


}
