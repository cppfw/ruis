#pragma once

#include <array>

#include <r4/rectangle.hpp>

#include "ResTexture.hpp"

#include "../ResourceManager.hpp"

#include "../Morda.hpp"

namespace morda{

/**
 * @brief Base image resource class.
 * 
 * %Resource description:
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
class ResImage : public Resource{
	friend class ResourceManager;
	
protected:
	ResImage(){}
	
public:
	ResImage(const ResImage& orig) = delete;
	ResImage& operator=(const ResImage& orig) = delete;
	
	/**
	 * @brief Texture created from the image resource.
	 * The texture which was created from an image resource.
	 * This texture is to be used on a quad to render the image.
	 */
	class QuadTexture : virtual public utki::shared{
		Vec2r dims_v;
	protected:
		QuadTexture(Vec2r dims) :
				dims_v(dims)
		{}
	public:
		/**
		 * @brief Get dimensions of the texture.
		 * @return Dimensions of the texture.
		 */
		const decltype(dims_v)& dims()const noexcept{
			return this->dims_v;
		}
		
		/**
		 * @brief Render a quad with this texture.
		 * @param matrix - transformation matrix to use for rendering.
		 * @param vao - vertex array to use for rendering.
		 */
		virtual void render(const Matr4r& matrix, const VertexArray& vao = *morda::inst().renderer().posTexQuad01VAO)const = 0;
	};

	/**
	 * @brief Get dimensions of this image in pixels for given Dots Per Inch resolution.
	 * @param dpi - dots per inch.
	 * @return Dimensions of the image in pixels.
	 */
	virtual Vec2r dims(real dpi = morda::Morda::inst().units.dpi())const noexcept = 0;
	
	/**
	 * @brief Get raster texture of given dimensions.
	 * @param forDims - dimensions request for raster texture.
	 *        If any of the dimensions is 0 then it will be adjusted to preserve aspect ratio.
	 *        If both dimensions are zero, then dimensions which are natural for the particular image will be used.
	 */
	virtual std::shared_ptr<const QuadTexture> get(Vec2r forDims = 0)const = 0;
private:
	static std::shared_ptr<ResImage> load(const stob::Node& chain, const papki::File& fi){
		return load(stob_to_puu(chain), fi);
	}
	static std::shared_ptr<ResImage> load(const puu::forest& desc, const papki::file& fi);
	
public:
	/**
	 * @brief Load image resource from image file.
	 * Files supported are PNG, JPG, SVG.
	 * @param fi - image file.
	 * @return Loaded resource.
	 */
	static std::shared_ptr<ResImage> load(const papki::File& fi);
};


/**
 * @brief Undocumented.
 */
class ResAtlasImage : public ResImage, public ResImage::QuadTexture{
	friend class ResImage;
	
	std::shared_ptr<ResTexture> tex;
	
	std::shared_ptr<VertexArray> vao;
	
public:
	ResAtlasImage(std::shared_ptr<ResTexture> tex, const Rectr& rect);
	ResAtlasImage(std::shared_ptr<ResTexture> tex);
	
	ResAtlasImage(const ResAtlasImage& orig) = delete;
	ResAtlasImage& operator=(const ResAtlasImage& orig) = delete;
	
	Vec2r dims(real dpi) const noexcept override{
		return this->ResImage::QuadTexture::dims();
	}
	
	virtual std::shared_ptr<const ResImage::QuadTexture> get(Vec2r forDim)const override{
		return this->sharedFromThis(this);
	}
	
	void render(const Matr4r& matrix, const VertexArray& vao) const override;
	
private:
	static std::shared_ptr<ResAtlasImage> load(const stob::Node& chain, const papki::File& fi){
		return load(stob_to_puu(chain), fi);
	}
	static std::shared_ptr<ResAtlasImage> load(const puu::forest& desc, const papki::file& fi);
};


}
