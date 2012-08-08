/* The MIT License:

Copyright (c) 2010-2012 Ivan Gagis

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

#include <tride/Vector3.hpp>
#include <tride/Rectangle2.hpp>

#include <ting/types.hpp>
#include <ting/Exc.hpp>
#include <ting/fs/File.hpp>

#include <GL/glew.h>

#include "GLTexture.hpp"
#include "../shaders/TexturingShader.hpp"



namespace morda{



class TexFont{
	struct Glyph{
		ting::StaticBuffer<tride::Vec2f, 4> verts;
		ting::StaticBuffer<tride::Vec2f, 4> texCoords;

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
	tride::Rect2f boundingBox;

public:
	TexFont(){}

	TexFont(ting::fs::File& fi, const wchar_t* chars, unsigned size, unsigned outline = 0){
		this->Load(fi, chars, size, outline);
	}

	void Load(ting::fs::File& fi, const wchar_t* chars, unsigned size, unsigned outline = 0);

	inline float FontSize()const{
		return this->fontSize;
	}

	//renders the string, returns resulting string advance
	inline float RenderString(TexturingShader& shader, const tride::Matr4f& matrix, const wchar_t* s)const{
		return this->RenderStringInternal(shader, matrix, s);
	}

	inline float RenderString(TexturingShader& shader, const tride::Matr4f& matrix, const std::wstring& s)const{
		return this->RenderStringInternal(shader, matrix, s.c_str());
	}

	inline float RenderString(TexturingShader& shader, const tride::Matr4f& matrix, const char* s)const{
		return this->RenderStringInternal(shader, matrix, s);
	}

	inline float RenderString(TexturingShader& shader, const tride::Matr4f& matrix, const std::string& s)const{
		return this->RenderStringInternal(shader, matrix, s.c_str());
	}

	inline const tride::Rect2f& FontBoundingBox()const{
		return this->boundingBox;
	}

	inline float StringAdvance(const wchar_t* s)const{
		return this->StringAdvanceInternal(s);
	}
	
	inline float StringAdvance(const std::wstring& s)const{
		return this->StringAdvanceInternal(s.c_str());
	}
	
	inline float StringAdvance(const char* s)const{
		return this->StringAdvanceInternal(s);
	}
	
	inline float StringAdvance(const std::string& s)const{
		return this->StringAdvanceInternal(s.c_str());
	}

	inline tride::Rect2f StringBoundingBox(const wchar_t* s)const{
		return this->StringBoundingBoxInternal(s);
	}
	
	inline tride::Rect2f StringBoundingBox(const std::wstring& s)const{
		return this->StringBoundingBoxInternal(s.c_str());
	}

	inline tride::Rect2f StringBoundingBox(const char* s)const{
		return this->StringBoundingBoxInternal(s);
	}

	inline tride::Rect2f StringBoundingBox(const std::string& s)const{
		return this->StringBoundingBoxInternal(s.c_str());
	}

	DEBUG_CODE( void RenderTex(TexturingShader& shader, const tride::Matr4f& matrix)const; )

private:

	float StringAdvanceInternal(const char* s)const;
	
	float StringAdvanceInternal(const wchar_t* s)const;

	tride::Rect2f StringBoundingBoxInternal(const char* s)const;
	
	tride::Rect2f StringBoundingBoxInternal(const wchar_t* s)const;
	
	float RenderStringInternal(TexturingShader& shader, const tride::Matr4f& matrix, const char* s)const;
	
	float RenderStringInternal(TexturingShader& shader, const tride::Matr4f& matrix, const wchar_t* s)const;	

	float RenderGlyphInternal(TexturingShader& shader, const tride::Matr4f& matrix, wchar_t ch)const;

};//~class TexFont



}//~namespace
