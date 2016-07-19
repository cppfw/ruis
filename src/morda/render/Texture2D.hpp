#pragma once

#include <utki/debug.hpp>

#include <kolme/Vector2.hpp>

#include "../config.hpp"

#include "../util/Image.hpp"

#include "../render/Render.hpp"

namespace morda{



/**
 * @brief Encapsulation of 2 dimensional texture.
 */
class Texture2D{
	friend class FrameBuffer;

	std::unique_ptr<utki::Void> tex;

	Vec2r dim_v;

	void Constructor(kolme::Vec2ui d, unsigned numChannels, const utki::Buf<std::uint8_t> data, Render::TexFilter_e minFilter, Render::TexFilter_e magFilter);
public:
	Texture2D(const Texture2D& tex) = delete;
	Texture2D& operator=(const Texture2D& tex) = delete;
	
	Texture2D(Texture2D&& tex) = default;
	Texture2D& operator=(Texture2D&&) = default;
	
	/**
	 * @brief Create texture from raster image.
	 * @param image - image to use for texture creation.
	 * @param minFilter - texture min-fileter. See OpenGL reference for details.
	 * @param magFilter - texture mag-fileter. See OpenGL reference for details.
	 */
	Texture2D(const Image& image, Render::TexFilter_e minFilter = Render::TexFilter_e::LINEAR, Render::TexFilter_e magFilter = Render::TexFilter_e::LINEAR){
		this->Constructor(image.dim(), image.numChannels(), image.buf(), minFilter, magFilter);
	}
	
	/**
	 * @brief Create a texture of given size and color depth.
	 * @param dimensions - size of the texture in pixels.
	 * @param numChannels - number of color channels. Should be from 1 to 4.
	 * @param minFilter - texture min-fileter. See OpenGL reference for details.
	 * @param magFilter - texture mag-fileter. See OpenGL reference for details.
	 */
	Texture2D(kolme::Vec2ui dimensions, unsigned numChannels, Render::TexFilter_e minFilter = Render::TexFilter_e::LINEAR, Render::TexFilter_e magFilter = Render::TexFilter_e::LINEAR){
		this->Constructor(dimensions, numChannels, nullptr, minFilter, magFilter);
	}
	
	/**
	 * @brief Create texture from array of RGBA pixels.
	 * @param width - width of the texture in pixels.
	 * @param rgbaPixels - array of RGBA pixels (32 bit values).
	 * @param minFilter - texture min-fileter. See OpenGL reference for details.
	 * @param magFilter - texture mag-fileter. See OpenGL reference for details.
	 */
	Texture2D(unsigned width, std::vector<std::uint32_t> rgbaPixels, Render::TexFilter_e minFilter = Render::TexFilter_e::LINEAR, Render::TexFilter_e magFilter = Render::TexFilter_e::LINEAR);

	/**
	 * @brief Create uninitialized Texture object.
	 */
	Texture2D(){}
	
	/**
	 * @brief Bind texture to texture unit.
	 * @param texUnitNum - number of the texture unit to bind to.
	 */
	void bind(unsigned texUnitNum = 0)const{
		Render::bindTexture(*this->tex, texUnitNum);
	}
	
	/**
	 * @brief Check if the texture is currently bound.
	 * @param texUnitNum - number of the texture unit to check.
	 * @return true if the texture is currently bound to given texture unit.
	 * @return false otherwise.
	 */
	bool isBound(unsigned texUnitNum = 0)const{
		return Render::isTextureBound(*this->tex, texUnitNum);
	}

	/**
	 * @brief Get dimensions of the texture.
	 * @return Dimensions of the texture.
	 */
	const decltype(dim_v)& dim()const noexcept{
		return this->dim_v;
	}

	/**
	 * @brief Check if this texture object is initialized (not empty).
	 * @return true if this texture object is initialized (not empty).
	 * @return false if this texture object is empty.
	 */
	explicit operator bool()const{
		return this->tex.operator bool();
	}
private:
};



}//~namespace
