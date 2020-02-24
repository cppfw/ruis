#pragma once

#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <list>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <r4/vector2.hpp>
#include <r4/rectangle.hpp>

#include <utki/Exc.hpp>
#include <papki/file.hpp>

#include "../config.hpp"

#include "../render/texture_2d.hpp"
#include "../render/vertex_array.hpp"

#include "font.hpp"


namespace morda{
/**
 * @brief A texture font.
 * This font implementation reads a Truetype font from 'ttf' file and renders given
 * set of characters to a texture.
 * Then, for rendering strings of text it renders
 * row of quads with texture coordinates corresponding to string characters on the texture.
 */
class TexFont : public font{
	mutable std::list<char32_t> lastUsedOrder;
	
	struct Glyph{
		morda::Vec2r topLeft;
		morda::Vec2r bottomRight;
		
		std::shared_ptr<vertex_array> vao;
		std::shared_ptr<texture_2d> tex;
		
		real advance;
		
		decltype(lastUsedOrder)::iterator lastUsedIter;
	};
	
	mutable std::unordered_map<char32_t, Glyph> glyphs;
	
	
	unsigned maxCached;

	struct FreeTypeLibWrapper{
		FT_Library lib;

		FreeTypeLibWrapper();
		~FreeTypeLibWrapper()noexcept;
	} freetype;

//	TRACE(<< "TexFont::Load(): FreeType library inited" << std::endl)

	struct FreeTypeFaceWrapper{
		FT_Face f;
		std::vector<std::uint8_t> fontFile;//the buffer should be alive as long as the Face is alive!!!

		FreeTypeFaceWrapper(FT_Library& lib, const papki::file& fi);
		~FreeTypeFaceWrapper()noexcept;
	} face;
	
	Glyph unknownGlyph;
	
	Glyph loadGlyph(char32_t c)const;
public:
	/**
	 * @brief Constructor.
	 * @param c - context to which this font belongs.
	 * @param fi - file interface to read Truetype font from, i.e. 'ttf' file.
	 * @param fontSize - size of the font in pixels.
	 * @param maxCached - maximum number of glyphs to cache.
	 */
	TexFont(std::shared_ptr<morda::context> c, const papki::file& fi, unsigned fontSize, unsigned maxCached);

	real get_advance(char32_t c)const override;
	
protected:
	real render_internal(const morda::Matr4r& matrix, r4::vec4f color, const std::u32string& str)const override;

	real get_advance_internal(const std::u32string& str)const override;

	morda::Rectr get_bounding_box_internal(const std::u32string& str)const override;
	
private:	
	real renderGlyphInternal(const morda::Matr4r& matrix, r4::vec4f color, char32_t ch)const;

	const Glyph& getGlyph(char32_t c)const;
};
}
