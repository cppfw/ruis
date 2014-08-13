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

#include <string>

namespace morda{


class Font{
	Font(const Font&);
	Font& operator=(const Font&);
	
protected:
	Font(){}
	
	virtual float RenderStringInternal(const morda::Matr4f& matrix, const ting::Buffer<std::uint32_t> utf32str)const = 0;
	
	virtual float StringAdvanceInternal(const ting::Buffer<std::uint32_t> utf32str)const = 0;
	
	virtual morda::Rect2f StringBoundingBoxInternal(const ting::Buffer<std::uint32_t> utf32str)const = 0;
public:
	virtual ~Font()NOEXCEPT{}
	
	virtual float Size()const NOEXCEPT = 0;
	
	//renders the string, returns resulting string advance
	float RenderString(const morda::Matr4f& matrix, ting::utf8::Iterator str)const;
	
	float RenderString(const morda::Matr4f& matrix, const ting::Buffer<std::uint32_t> utf32str)const{
		return this->RenderStringInternal(matrix, utf32str);
	}
	
	float RenderString(const morda::Matr4f& matrix, const char* str)const{
		return this->RenderString(matrix, ting::utf8::Iterator(str));
	}
	
	float RenderString(const morda::Matr4f& matrix, const std::string& str)const{
		return this->RenderString(matrix, str.c_str());
	}
	
	
	
	float StringAdvance(ting::utf8::Iterator str)const;
	
	float StringAdvance(const ting::Buffer<std::uint32_t> utf32str)const{
		return this->StringAdvanceInternal(utf32str);
	}
	
	float StringAdvance(const char* str)const{
		return this->StringAdvance(ting::utf8::Iterator(str));
	}
	
	float StringAdvance(const std::string& str)const{
		return this->StringAdvance(str.c_str());
	}
	
	
	
	morda::Rect2f StringBoundingBox(ting::utf8::Iterator str)const;
	
	morda::Rect2f StringBoundingBox(const ting::Buffer<std::uint32_t> utf32str)const{
		return this->StringBoundingBoxInternal(utf32str);
	}
	
	morda::Rect2f StringBoundingBox(const char* str)const{
		return this->StringBoundingBox(ting::utf8::Iterator(str));
	}
	
	morda::Rect2f StringBoundingBox(const std::string& str)const{
		return this->StringBoundingBox(str.c_str());
	}
	
	
	
private:

};

}
