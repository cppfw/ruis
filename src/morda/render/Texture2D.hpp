/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "../config.hpp"

#include <utki/debug.hpp>

#include "../util/Vector2.hpp"

#include "../util/Image.hpp"

#include "../render/Render.hpp"

namespace morda{



class Texture2D{
	friend class FrameBuffer;

	std::unique_ptr<utki::Void> tex;

	morda::Vec2r dim;

	void Constructor(Vec2ui d, unsigned numChannels, const utki::Buf<std::uint8_t> data, Render::ETexFilter minFilter, Render::ETexFilter magFilter);
public:
	Texture2D(const Texture2D& tex) = delete;
	Texture2D& operator=(const Texture2D& tex) = delete;
	
	Texture2D(Texture2D&& tex) = default;
	Texture2D& operator=(Texture2D&&) = default;
	
	Texture2D(const Image& image, Render::ETexFilter minFilter = Render::ETexFilter::LINEAR, Render::ETexFilter magFilter = Render::ETexFilter::LINEAR){
		this->Constructor(image.dim(), image.numChannels(), image.buf(), minFilter, magFilter);
	}
	
	Texture2D(Vec2ui dimensions, unsigned numChannels, Render::ETexFilter minFilter = Render::ETexFilter::LINEAR, Render::ETexFilter magFilter = Render::ETexFilter::LINEAR){
		this->Constructor(dimensions, numChannels, nullptr, minFilter, magFilter);
	}

	Texture2D(){}

	void bind()const{
		Render::bindTexture(*this->tex, 0);
	}
	
	void bind(unsigned texUnitNum)const{
		Render::bindTexture(*this->tex, texUnitNum);
	}

	morda::Vec2r Dim()const noexcept{
		return this->dim;
	}

	explicit operator bool()const{
		return this->tex.operator bool();
	}
private:
};



}//~namespace
