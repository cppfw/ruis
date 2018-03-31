#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H


#include <utki/debug.hpp>

#include "../util/RasterImage.hpp"
#include "../util/util.hpp"

#include "TexFont.hpp"
#include "../Morda.hpp"



using namespace morda;



namespace{

constexpr const char32_t unknownChar_c = 0xfffd;

}



void TexFont::load(const papki::File& fi, const std::u32string& chars, unsigned fontSize){
	std::u32string fontChars = chars;
	fontChars.append(1, unknownChar_c);
	
//	TRACE(<< "TexFont::Load(): enter" << std::endl)

	this->glyphs.clear();//clear glyphs map if some other font was loaded previously
	
	class FreeTypeLibWrapper{
		FT_Library lib;// handle to freetype library object
	public:
		FreeTypeLibWrapper(){
			if(FT_Init_FreeType(&this->lib)){
				throw utki::Exc("TexFont::Load(): unable to init freetype library");
			}
		}
		~FreeTypeLibWrapper()noexcept{
			FT_Done_FreeType(this->lib);
		}
		operator FT_Library& (){
			return this->lib;
		}
	} library;

//	TRACE(<< "TexFont::Load(): FreeType library inited" << std::endl)

	class FreeTypeFaceWrapper{
		FT_Face face; // handle to face object
		std::vector<std::uint8_t> fontFile;//the buffer should be alive as long as the Face is alive!!!
	public:
		FreeTypeFaceWrapper(FT_Library& lib, const papki::File& fi){
			this->fontFile = fi.loadWholeFileIntoMemory();
			if(FT_New_Memory_Face(lib, &*this->fontFile.begin(), int(this->fontFile.size()), 0/* face_index */, &this->face) != 0){
				throw utki::Exc("TexFont::Load(): unable to crate font face object");
			}
		}
		~FreeTypeFaceWrapper()noexcept{
			FT_Done_Face(this->face);
		}
		operator FT_Face& (){
			return this->face;
		}
	} face(library, fi);

//	TRACE(<< "TexFont::Load(): FreeType font face loaded" << std::endl)

	//set character size in pixels
	{
		FT_Error error = FT_Set_Pixel_Sizes(
				face,// handle to face object
				0,// pixel_width (0 means "same as height")
				fontSize
			); // pixel_height

		if(error != 0){
			throw utki::Exc("TexFont::Load(): unable to set char size");
		}
	}

//	TRACE(<< "TexFont::Load(): entering for loop" << std::endl)

	auto indexBuffer = morda::inst().renderer().quadIndices;
	
	//print all the glyphs to the image
	for(auto c = fontChars.begin(); c != fontChars.end(); ++c){
		if(FT_Load_Char(static_cast<FT_Face&>(face), FT_ULong(*c), FT_LOAD_RENDER) != 0){
			throw utki::Exc("TexFont::Load(): unable to load char");
		}
		
//		TRACE(<< "TexFont::Load(): char loaded" << std::endl)

		FT_GlyphSlot slot = static_cast<FT_Face&>(face)->glyph;

		if(!slot->bitmap.buffer){//if glyph is empty (e.g. space character)
			Glyph &g = this->glyphs[*c];
			g.advance = float(slot->metrics.horiAdvance) / (64.0f);
			g.vao = morda::inst().renderer().posTexQuad01VAO;
			std::uint32_t p = 0;
			g.tex = morda::inst().renderer().factory->createTexture2D(1, utki::Buf<std::uint32_t>(&p, 1));
//			ASSERT(g.verts.size() == g.texCoords.size())
//			for(unsigned i = 0; i < g.verts.size(); ++i){
//				g.verts[i].set(0);
//				g.texCoords[i].set(0);
//			}
			continue;
		}
		RasterImage glyphim(kolme::Vec2ui(slot->bitmap.width, slot->bitmap.rows), RasterImage::ColorDepth_e::GREY, slot->bitmap.buffer);

		RasterImage im(kolme::Vec2ui(glyphim.dim().x, glyphim.dim().y), RasterImage::ColorDepth_e::GREYA);
		im.clear(std::uint8_t(0xff));
		im.blit(0, 0, glyphim, 1, 0);
		
//		TRACE(<< "TexFont::Load(): glyph image created" << std::endl)

		//record glyph information
		{
			FT_Glyph_Metrics *m = &slot->metrics;
			
			Glyph &g = this->glyphs[*c];
			g.advance = real(m->horiAdvance) / (64.0f);
			
			std::array<kolme::Vec2f, 4> verts;
			verts[0] = (morda::Vec2r(real(m->horiBearingX), -real(m->horiBearingY)) / (64.0f));
			verts[1] = (morda::Vec2r(real(m->horiBearingX), real(m->height - m->horiBearingY)) / (64.0f));
			verts[2] = (morda::Vec2r(real(m->horiBearingX + m->width), real(m->height - m->horiBearingY)) / (64.0f));
			verts[3] = (morda::Vec2r(real(m->horiBearingX + m->width), -real(m->horiBearingY)) / (64.0f));
			
			g.topLeft = verts[0];
			g.bottomRight = verts[2];
			
			auto& r = morda::inst().renderer();
			g.vao = r.factory->createVertexArray(
					{
						r.factory->createVertexBuffer(utki::wrapBuf(verts)),
						morda::inst().renderer().quad01VBO
					},
					indexBuffer,
					VertexArray::Mode_e::TRIANGLE_STRIP
				);
			g.tex = morda::inst().renderer().factory->createTexture2D(
					morda::numChannelsToTexType(im.numChannels()),
					im.dim(),
					im.buf()
				);
		}
	}
	
	using std::ceil;
	
	this->height_v = ceil((static_cast<FT_Face&>(face)->size->metrics.height) / 64.0f);
	this->descender_v = -ceil((static_cast<FT_Face&>(face)->size->metrics.descender) / 64.0f);
	this->ascender_v = ceil((static_cast<FT_Face&>(face)->size->metrics.ascender) / 64.0f);

	TRACE(<< "TexFont::load(): height_v = " << this->height_v << std::endl)
}

const TexFont::Glyph& TexFont::findGlyph(char32_t c)const{
	auto i = this->glyphs.find(c);
	if(i == this->glyphs.end()){
		return this->glyphs.at(unknownChar_c);
	}
	return i->second;
}



real TexFont::renderGlyphInternal(const morda::Matr4r& matrix, kolme::Vec4f color, char32_t ch)const{
	const Glyph& g = this->findGlyph(ch);
	
	morda::inst().renderer().shader->colorPosTex->render(matrix, *g.vao, color, *g.tex);

	return g.advance;
}



real TexFont::stringAdvanceInternal(const std::u32string& str)const{
	real ret = 0;

	auto s = str.begin();
	
	for(; s != str.end(); ++s){
		try{
			const Glyph& g = this->findGlyph(*s);
			ret += g.advance;
		}catch(std::out_of_range&){
			//ignore
		}
	}

	return ret;
}



morda::Rectr TexFont::stringBoundingBoxInternal(const std::u32string& str)const{
	morda::Rectr ret;

	if(str.size() == 0){
		ret.p.set(0);
		ret.d.set(0);
		return ret;
	}

	auto s = str.begin();

	real curAdvance;

	real left, right, top, bottom;
	//init with bounding box of the first glyph
	{
		const Glyph& g = this->findGlyph(*s);
		left = g.topLeft.x;
		right = g.bottomRight.x;
		top = g.topLeft.y;
		bottom = g.bottomRight.y;
		curAdvance = g.advance;
		++s;
	}

	for(; s != str.end(); ++s){
		auto i = this->glyphs.find(*s);
		
#ifdef DEBUG
		if(i == this->glyphs.end()){
			TRACE(<< "TexFont::StringBoundingLineInternal(): Character is not loaded, scan code = 0x" << std::hex << *s << std::endl)
		}
#endif

		const Glyph& g = i == this->glyphs.end() ? this->glyphs.at(unknownChar_c) : i->second;

		top = std::min(g.topLeft.y, top);
		bottom = std::max(g.bottomRight.y, bottom);
		left = std::min(curAdvance + g.topLeft.x, left);
		right = std::max(curAdvance + g.bottomRight.x, right);

		curAdvance += g.advance;
	}

	ret.p.x = left;
	ret.p.y = top;
	ret.d.x = right - left;
	ret.d.y = -(top - bottom);

	ASSERT(ret.d.x >= 0)
	ASSERT(ret.d.y >= 0)
//	TRACE(<< "TexFont::stringBoundingBoxInternal(): ret = " << ret << std::endl)
	return ret;
}



real TexFont::renderStringInternal(const morda::Matr4r& matrix, kolme::Vec4f color, const std::u32string& str)const{
	if(str.size() == 0){
		return 0;
	}
	
	applySimpleAlphaBlending();

	real ret = 0;

	morda::Matr4r matr(matrix);

	auto s = str.begin();

	for(; s != str.end(); ++s){
		try{
			real advance = this->renderGlyphInternal(matr, color, *s);
			ret += advance;
			matr.translate(advance, 0);
		}catch(std::out_of_range&){
			//ignore
		}
	}
	

	return ret;
}



real TexFont::charAdvance(char32_t c) const{
	auto i = this->glyphs.find(c);
	if(i == this->glyphs.end()){
		ASSERT(this->glyphs.find(unknownChar_c) != this->glyphs.end())
		return this->glyphs.at(unknownChar_c).advance;
	}
	
	const Glyph& g = i->second;
	return g.advance;
}
