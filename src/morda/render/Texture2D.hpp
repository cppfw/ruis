/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "../config.hpp"

#include <utki/debug.hpp>

#include <kolme/Vector2.hpp>

#include "../util/Image.hpp"

#include "../render/Render.hpp"

namespace morda{



class Texture2D{
	friend class FrameBuffer;

	std::unique_ptr<utki::Void> tex;

	Vec2r dim_var;

	void Constructor(kolme::Vec2ui d, unsigned numChannels, const utki::Buf<std::uint8_t> data, Render::TexFilter_e minFilter, Render::TexFilter_e magFilter);
public:
	Texture2D(const Texture2D& tex) = delete;
	Texture2D& operator=(const Texture2D& tex) = delete;
	
	Texture2D(Texture2D&& tex) = default;
	Texture2D& operator=(Texture2D&&) = default;
	
	Texture2D(const Image& image, Render::TexFilter_e minFilter = Render::TexFilter_e::LINEAR, Render::TexFilter_e magFilter = Render::TexFilter_e::LINEAR){
		this->Constructor(image.dim(), image.numChannels(), image.buf(), minFilter, magFilter);
	}
	
	Texture2D(kolme::Vec2ui dimensions, unsigned numChannels, Render::TexFilter_e minFilter = Render::TexFilter_e::LINEAR, Render::TexFilter_e magFilter = Render::TexFilter_e::LINEAR){
		this->Constructor(dimensions, numChannels, nullptr, minFilter, magFilter);
	}
	
	Texture2D(unsigned width, std::vector<std::uint32_t> rgbaPixels, Render::TexFilter_e minFilter = Render::TexFilter_e::LINEAR, Render::TexFilter_e magFilter = Render::TexFilter_e::LINEAR);

	Texture2D(){}
	
	void bind(unsigned texUnitNum = 0)const{
		Render::bindTexture(*this->tex, texUnitNum);
	}
	
	bool isBound(unsigned texUnitNum = 0)const{
		return Render::isTextureBound(*this->tex, texUnitNum);
	}

	const decltype(dim_var)& dim()const noexcept{
		return this->dim_var;
	}

	explicit operator bool()const{
		return this->tex.operator bool();
	}
private:
};



}//~namespace
