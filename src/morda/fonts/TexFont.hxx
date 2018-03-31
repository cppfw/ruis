#pragma once

#include <map>
#include <sstream>
#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <kolme/Vector2.hpp>
#include <kolme/Rectangle.hpp>

#include <utki/Exc.hpp>
#include <papki/File.hpp>

#include "../config.hpp"

#include "../render/Texture2D.hpp"
#include "../render/VertexArray.hpp"

#include "Font.hpp"


namespace morda{
/**
 * @brief A texture font.
 * This font implementation reads a Truetype font from 'ttf' file and renders given
 * set of characters to a texture.
 * Then, for rendering strings of text it renders
 * row of quads with texture coordinates corresponding to string characters on the texture.
 */
class TexFont : public Font{
	struct Glyph{
		morda::Vec2r topLeft;
		morda::Vec2r bottomRight;
		
		std::shared_ptr<VertexArray> vao;
		std::shared_ptr<Texture2D> tex;
		
		real advance;
	};

	typedef std::map<char32_t, Glyph> T_GlyphsMap;
	typedef T_GlyphsMap::iterator T_GlyphsIter;
	mutable T_GlyphsMap glyphs;

	struct FreeTypeLibWrapper{
		FT_Library lib;

		FreeTypeLibWrapper();
		~FreeTypeLibWrapper()noexcept;
	} freetype;

//	TRACE(<< "TexFont::Load(): FreeType library inited" << std::endl)

	struct FreeTypeFaceWrapper{
		FT_Face f;
		std::vector<std::uint8_t> fontFile;//the buffer should be alive as long as the Face is alive!!!

		FreeTypeFaceWrapper(FT_Library& lib, const papki::File& fi);
		~FreeTypeFaceWrapper()noexcept;
	} face;
	
	Glyph unknownGlyph;
	
	Glyph loadGlyph(char32_t c)const;
public:
	/**
	 * @brief Constructor.
	 * @param fi - file interface to read Truetype font from, i.e. 'ttf' file.
	 * @param fontSize - size of the font in pixels.
	 */
	TexFont(const papki::File& fi, unsigned fontSize);

	real charAdvance(char32_t c) const override;
	
protected:
	real renderStringInternal(const morda::Matr4r& matrix, kolme::Vec4f color, const std::u32string& str)const override;

	real stringAdvanceInternal(const std::u32string& str)const override;

	morda::Rectr stringBoundingBoxInternal(const std::u32string& str)const override;
	
private:	
	real renderGlyphInternal(const morda::Matr4r& matrix, kolme::Vec4f color, char32_t ch)const;

	const Glyph& getGlyph(char32_t c)const;
};
}
