/* The MIT License:

Copyright (c) 2012-2015 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "../config.hpp"

#include <ting/debug.hpp>

#include "../util/Vector2.hpp"

#include "../util/Image.hpp"

#include "../render/Render.hpp"

namespace morda{



class Texture2D{
	friend class FrameBuffer;
	
	Texture2D(const Texture2D& tex) = delete;
	Texture2D& operator=(const Texture2D& tex) = delete;

	std::unique_ptr<ting::Void> tex;

	morda::Vec2r dim;

	void Constructor(Vec2ui d, unsigned numChannels, ting::Buffer<const std::uint8_t> data, Render::ETexFilter minFilter, Render::ETexFilter magFilter);
public:
	Texture2D& operator=(Texture2D&&) = default;
	
	Texture2D(const Image& image, Render::ETexFilter minFilter = Render::ETexFilter::LINEAR, Render::ETexFilter magFilter = Render::ETexFilter::LINEAR){
		this->Constructor(image.Dim(), image.NumChannels(), image.Buf(), minFilter, magFilter);
	}
	
	Texture2D(Vec2ui dimensions, unsigned numChannels, Render::ETexFilter minFilter = Render::ETexFilter::LINEAR, Render::ETexFilter magFilter = Render::ETexFilter::LINEAR){
		this->Constructor(dimensions, numChannels, ting::Buffer<const std::uint8_t>(), minFilter, magFilter);
	}

	Texture2D(){}

	void Bind()const{
		Render::bindTexture(*this->tex, 0);
	}
	
	void Bind(unsigned texUnitNum)const{
		Render::bindTexture(*this->tex, texUnitNum);
	}

	morda::Vec2r Dim()const NOEXCEPT{
		return this->dim;
	}

private:
};



}//~namespace
