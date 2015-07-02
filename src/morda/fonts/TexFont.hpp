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

#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGLES
#	include <GLES2/gl2.h>
#elif M_MORDA_RENDER == M_MORDA_RENDER_OPENGL
#	include <GL/glew.h>
#elif M_MORDA_RENDER == M_MORDA_RENDER_DIRECTX

#else
#	error "unknown render API"
#endif

#include "../util/Texture2D.hpp"
#include "Font.hpp"

#include "../shaders/PosTexShader.hpp"



namespace morda{



class TexFont : public Font{
	struct Glyph{
		std::array<morda::Vec2f, 4> verts;
		std::array<morda::Vec2f, 4> texCoords;

		real advance;
	};

	Texture2D tex;

	typedef std::map<wchar_t, Glyph> T_GlyphsMap;
	typedef T_GlyphsMap::iterator T_GlyphsIter;
	T_GlyphsMap glyphs;

public:
	
	TexFont(const ting::fs::File& fi, const ting::Buffer<std::uint32_t> chars, unsigned size, unsigned outline = 0){
		this->Load(fi, chars, size, outline);
	}

	~TexFont()NOEXCEPT{}
	
	void Load(const ting::fs::File& fi, const ting::Buffer<std::uint32_t> chars, unsigned size, unsigned outline = 0);

	
	real RenderStringInternal(PosTexShader& shader, const morda::Matr4r& matrix, ting::Buffer<const std::uint32_t> utf32str)const override;

	
	real StringAdvanceInternal(ting::Buffer<const std::uint32_t> utf32str)const override;

	morda::Rect2r StringBoundingBoxInternal(ting::Buffer<const std::uint32_t> utf32str)const override;

	DEBUG_CODE( void RenderTex(PosTexShader& shader, const morda::Matr4r& matrix)const; )


	real CharAdvance(std::uint32_t c) const override;

	
private:

	real RenderGlyphInternal(PosTexShader& shader, const morda::Matr4r& matrix, std::uint32_t ch)const;

};//~class TexFont



}//~namespace
