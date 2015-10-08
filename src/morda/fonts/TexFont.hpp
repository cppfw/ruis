/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include <map>
#include <sstream>
#include <stdexcept>

#include <kolme/Vector2.hpp>
#include <kolme/Rectangle.hpp>

#include <utki/Exc.hpp>
#include <papki/File.hpp>

#include "../config.hpp"

#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGLES
#	include <GLES2/gl2.h>
#elif M_MORDA_RENDER == M_MORDA_RENDER_OPENGL
#	include <GL/glew.h>
#elif M_MORDA_RENDER == M_MORDA_RENDER_DIRECTX

#else
#	error "unknown render API"
#endif

#include "../render/Texture2D.hpp"
#include "Font.hpp"

#include "../shaders/PosTexShader.hpp"



namespace morda{



class TexFont : public Font{
	struct Glyph{
		std::array<kolme::Vec2f, 4> verts;
		std::array<kolme::Vec2f, 4> texCoords;

		real advance;
	};

	Texture2D tex;

	typedef std::map<wchar_t, Glyph> T_GlyphsMap;
	typedef T_GlyphsMap::iterator T_GlyphsIter;
	T_GlyphsMap glyphs;

public:
	
	TexFont(const papki::File& fi, const utki::Buf<std::uint32_t> chars, unsigned size, unsigned outline = 0){
		this->Load(fi, chars, size, outline);
	}

	~TexFont()noexcept{}
	
	void Load(const papki::File& fi, const utki::Buf<std::uint32_t> chars, unsigned size, unsigned outline = 0);

	
	real RenderStringInternal(PosTexShader& shader, const morda::Matr4r& matrix, const utki::Buf<std::uint32_t> utf32str)const override;

	
	real StringAdvanceInternal(const utki::Buf<std::uint32_t> utf32str)const override;

	morda::Rectr StringBoundingBoxInternal(const utki::Buf<std::uint32_t> utf32str)const override;

	DEBUG_CODE( void RenderTex(PosTexShader& shader, const morda::Matr4r& matrix)const; )


	real CharAdvance(std::uint32_t c) const override;

	
private:

	real RenderGlyphInternal(PosTexShader& shader, const morda::Matr4r& matrix, std::uint32_t ch)const;

};//~class TexFont



}//~namespace
