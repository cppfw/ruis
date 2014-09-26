/* The MIT License:

Copyright (c) 2014 Ivan Gagis <igagis@gmail.com>

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

#include <ting/Buffer.hpp>
#include <ting/utf8.hpp>

#include "../util/Matrix4.hpp"
#include "../util/Rectangle2.hpp"

#include "../shaders/PosTexShader.hpp"

#include <string>

namespace morda{


class Font{
	Font(const Font&);
	Font& operator=(const Font&);
	
protected:
	Font(){}
	
	virtual real RenderStringInternal(PosTexShader& shader, const morda::Matr4r& matrix, ting::Buffer<const std::uint32_t> utf32str)const = 0;
	
	virtual real StringAdvanceInternal(ting::Buffer<const std::uint32_t> utf32str)const = 0;
	
	virtual morda::Rect2r StringBoundingBoxInternal(ting::Buffer<const std::uint32_t> utf32str)const = 0;
public:
	virtual ~Font()NOEXCEPT{}
	
	virtual real Size()const NOEXCEPT = 0;
	
	//renders the string, returns resulting string advance
	real RenderString(PosTexShader& shader, const morda::Matr4r& matrix, ting::utf8::Iterator str)const;
	
	real RenderString(PosTexShader& shader, const morda::Matr4r& matrix, ting::Buffer<const std::uint32_t> utf32str)const{
		return this->RenderStringInternal(shader, matrix, utf32str);
	}
	
	real RenderString(PosTexShader& shader, const morda::Matr4r& matrix, const char* str)const{
		return this->RenderString(shader, matrix, ting::utf8::Iterator(str));
	}
	
	real RenderString(PosTexShader& shader, const morda::Matr4r& matrix, const std::string& str)const{
		return this->RenderString(shader, matrix, str.c_str());
	}
	
	
	
	real StringAdvance(ting::utf8::Iterator str)const;
	
	real StringAdvance(ting::Buffer<const std::uint32_t> utf32str)const{
		return this->StringAdvanceInternal(utf32str);
	}
	
	real StringAdvance(const char* str)const{
		return this->StringAdvance(ting::utf8::Iterator(str));
	}
	
	real StringAdvance(const std::string& str)const{
		return this->StringAdvance(str.c_str());
	}
	
	
	
	virtual real CharAdvance(std::uint32_t c)const = 0;
	
	
	
	morda::Rect2r StringBoundingBox(ting::utf8::Iterator str)const;
	
	morda::Rect2r StringBoundingBox(ting::Buffer<const std::uint32_t> utf32str)const{
		return this->StringBoundingBoxInternal(utf32str);
	}
	
	morda::Rect2r StringBoundingBox(const char* str)const{
		return this->StringBoundingBox(ting::utf8::Iterator(str));
	}
	
	morda::Rect2r StringBoundingBox(const std::string& str)const{
		return this->StringBoundingBox(str.c_str());
	}
	
	
	
private:

};

}
