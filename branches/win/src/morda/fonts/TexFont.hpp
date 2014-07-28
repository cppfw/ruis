/* The MIT License:

Copyright (c) 2010-2014 Ivan Gagis

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

#include <map>
#include <sstream>
#include <stdexcept>

#include "../util/Vector2.hpp"
#include "../util/Rectangle2.hpp"

#include <ting/types.hpp>
#include <ting/Exc.hpp>
#include <ting/fs/File.hpp>
#include <ting/util.hpp>

#include "../config.hpp"

#ifdef M_MORDA_OGLES2
#	include <GLES2/gl2.h>
#else
#	include <GL/glew.h>
#endif

#include "../util/GLTexture.hpp"
#include "Font.hpp"

#include "../shaders/TexturingShader.hpp"



namespace morda{



class TexFont : public Font{
	struct Glyph{
		ting::StaticBuffer<morda::Vec2f, 4> verts;
		ting::StaticBuffer<morda::Vec2f, 4> texCoords;

		float advance;
	};

	GLTexture tex;

	typedef std::map<wchar_t, Glyph> T_GlyphsMap;
	typedef T_GlyphsMap::iterator T_GlyphsIter;
	T_GlyphsMap glyphs;

	float fontSize;
public:

private:
	//Bounding box holds the dimensions of the largest loaded glyph.
	morda::Rect2f boundingBox;

public:
	
	TexFont(ting::fs::File& fi, const ting::Buffer<const ting::u32>& chars, unsigned size, unsigned outline = 0){
		this->Load(fi, chars, size, outline);
	}

	~TexFont()throw(){}
	
	void Load(ting::fs::File& fi, const ting::Buffer<const ting::u32>& chars, unsigned size, unsigned outline = 0);

	float Size()const throw() OVERRIDE{
		return this->fontSize;
	}

	
	float RenderStringInternal(const morda::Matr4f& matrix, const ting::Buffer<const ting::u32>& utf32str)const OVERRIDE;

	const morda::Rect2f& FontBoundingBox()const{
		return this->boundingBox;
	}

	
	float StringAdvanceInternal(const ting::Buffer<const ting::u32>& utf32str)const OVERRIDE;

	morda::Rect2f StringBoundingBoxInternal(const ting::Buffer<const ting::u32>& utf32str)const OVERRIDE;

	DEBUG_CODE( void RenderTex(TexturingShader& shader, const morda::Matr4f& matrix)const; )

private:

	float RenderGlyphInternal(TexturingShader& shader, const morda::Matr4f& matrix, ting::u32 ch)const;

};//~class TexFont



}//~namespace
