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

	typedef std::map<char32_t, Glyph> T_GlyphsMap;
	typedef T_GlyphsMap::iterator T_GlyphsIter;
	T_GlyphsMap glyphs;

public:
	
	TexFont(const papki::File& fi, const utki::Buf<std::uint32_t> chars, unsigned size, unsigned outline = 0){
		this->load(fi, chars, size, outline);
	}

	~TexFont()noexcept{}
	
	void load(const papki::File& fi, const utki::Buf<std::uint32_t> chars, unsigned size, unsigned outline = 0);

	
	real renderStringInternal(PosTexShader& shader, const morda::Matr4r& matrix, const std::u32string& str)const override;

	
	real stringAdvanceInternal(const std::u32string& str)const override;

	morda::Rectr stringBoundingBoxInternal(const std::u32string& str)const override;

	DEBUG_CODE( void renderTex(PosTexShader& shader, const morda::Matr4r& matrix)const; )


	real charAdvance(std::uint32_t c) const override;

	
private:

	real renderGlyphInternal(PosTexShader& shader, const morda::Matr4r& matrix, std::uint32_t ch)const;

};



}//~namespace
