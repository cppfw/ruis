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

#include "../shaders/PosTexShader.hpp"

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
		std::array<kolme::Vec2f, 4> verts;
		std::array<kolme::Vec2f, 4> texCoords;

		real advance;
	};

	Texture2D tex;

	typedef std::map<char32_t, Glyph> T_GlyphsMap;
	typedef T_GlyphsMap::iterator T_GlyphsIter;
	T_GlyphsMap glyphs;

public:
	/**
	 * @brief Constructor.
	 * @param fi - file interface to read Truetype font from, i.e. 'ttf' file.
	 * @param chars - set of characters to put to the font texture.
	 * @param size - size of the font in pixels.
	 * @param outline - thickness of the outline effect.
	 */
	TexFont(const papki::File& fi, const utki::Buf<std::uint32_t> chars, unsigned size, unsigned outline = 0){
		this->load(fi, chars, size, outline);
	}

	~TexFont()noexcept{}

	
	real renderStringInternal(PosTexShader& shader, const morda::Matr4r& matrix, const std::u32string& str)const override;

	
	real stringAdvanceInternal(const std::u32string& str)const override;

	morda::Rectr stringBoundingBoxInternal(const std::u32string& str)const override;

//	void renderTex(PosTexShader& shader, const morda::Matr4r& matrix)const{
//		morda::Matr4r matr(matrix);
//		matr.scale(this->tex.dim());
//		shader.setMatrix(matr);
//
//		this->tex.bind();
//
//		shader.render(utki::wrapBuf(PosShader::quad01Fan), utki::wrapBuf(shader.quadFanTexCoords));
//	}


	real charAdvance(std::uint32_t c) const override;

	
private:

	void load(const papki::File& fi, const utki::Buf<std::uint32_t> chars, unsigned size, unsigned outline = 0);
	
	real renderGlyphInternal(PosTexShader& shader, const morda::Matr4r& matrix, std::uint32_t ch)const;

};



}//~namespace
