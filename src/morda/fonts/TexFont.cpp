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

static void BlitIfGreater(RasterImage& dst, unsigned dstChan, const RasterImage& src, unsigned srcChan, unsigned x, unsigned y){
	ASSERT(dst.buf().size())
	ASSERT(dstChan < dst.numChannels())
	ASSERT(srcChan < src.numChannels())

	unsigned blitAreaW = std::min(src.dim().x, dst.dim().x - x);
	unsigned blitAreaH = std::min(src.dim().y, dst.dim().y - y);

	for(unsigned j = 0; j < blitAreaH; ++j){
		for(unsigned i = 0; i < blitAreaW; ++i){
			std::uint8_t &d = dst.pixChan(i + x, j + y, dstChan);
			std::uint8_t s = src.pixChan(i, j, srcChan);
			if(s > d){
				d = s;
			}
		}
	}
}


static unsigned FindNextPowOf2(unsigned n){
	unsigned ret = 1;
	while(ret <= n){
		ret *= 2;
	}
	return ret;
}

const unsigned DXGap = 1;
const unsigned DYGap = 1;

constexpr const char32_t unknownChar_c = 0xfffd;

}



void TexFont::load(const papki::File& fi, const std::u32string& chars, unsigned fontSize, unsigned outline){
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

	unsigned maxTexSize = morda::inst().renderer().maxTextureSize;

	//guess for texture width
	unsigned texWidth;
	texWidth = std::max(unsigned(128), FindNextPowOf2(unsigned(fontChars.size() / 8) * fontSize)); //divide by 8 is a good guess that all font characters will be placed in 8 rows on texture
	texWidth = std::min(std::min(maxTexSize, unsigned(1024)), texWidth); //clamp width to min of max texture size and 1024

	unsigned curTexHeight = FindNextPowOf2(fontSize);//first guess of texture height
	unsigned curX = DXGap;
	unsigned curY = DYGap;
	unsigned maxHeightInRow = 0;

	RasterImage texImg(kolme::Vec2ui(texWidth, curTexHeight), RasterImage::ColorDepth_e::GREYA);
	//clear the image because image buffer may contain trash data
	//and glyphs will have artifacts on their edges
	texImg.clear();

	//init bounding box to invalid values
	float left = 1000000;
	float right = -1000000;
	float top = 1000000;
	float bottom = -1000000;

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

		RasterImage im(kolme::Vec2ui(glyphim.dim().x + 2 * outline, glyphim.dim().y + 2 * outline), RasterImage::ColorDepth_e::GREYA);
		im.clear();
		if(outline == 0){
			im.blit(0, 0, glyphim, 1, 0);
		}else{
			im.blit(outline, outline, glyphim, 0, 0);

			for(unsigned y = 0; y < 2 * outline + 1; ++y){
				for(unsigned x = 0; x < 2 * outline + 1; ++x){
					int dx = int(x) - int(outline);
					int dy = int(y) - int(outline);
					using utki::pow2;
					if(pow2(dx) + pow2(dy) <= int(pow2(outline))){
						BlitIfGreater(im, 1, glyphim, 0, x, y);
					}
				}
			}
		}
		
//		TRACE(<< "TexFont::Load(): glyph image created" << std::endl)

		if(texImg.dim().x < curX + im.dim().x + DXGap){
			curX = DXGap;
			curY += maxHeightInRow + DYGap;
			maxHeightInRow = 0;
		}

		if(texImg.dim().y < curY + im.dim().y + DYGap){
			//grow texture size
			unsigned newHeight = FindNextPowOf2(curY + im.dim().y + DYGap);
			if(newHeight > maxTexSize){//if it is impossible to grow texture image height anymore
				throw utki::Exc("TexFont::Load(): there's not enough room on the texture for all the glyphs of the requested size");
			}

			//TODO: optimize somehow?
			//resize texture image
			RasterImage copy(texImg);
			texImg.init(kolme::Vec2ui(copy.dim().x, newHeight), copy.colorDepth());
			texImg.clear();
			texImg.blit(0, 0, copy);
		}

		if(im.dim().y > maxHeightInRow){
			maxHeightInRow = im.dim().y;
		}

		//record glyph information
		{
			FT_Glyph_Metrics *m = &slot->metrics;
			
			Glyph &g = this->glyphs[*c];
			g.advance = real(m->horiAdvance) / (64.0f);
			
			ASSERT(outline < (unsigned(-1) >> 1))
			g.verts[0] = (morda::Vec2r(real(m->horiBearingX), -real(m->horiBearingY - m->height)) / (64.0f)) + morda::Vec2r(-real(outline), -real(outline));
			g.verts[1] = (morda::Vec2r(real(m->horiBearingX + m->width), -real(m->horiBearingY - m->height)) / (64.0f)) + morda::Vec2r(real(outline), -real(outline));
			g.verts[2] = (morda::Vec2r(real(m->horiBearingX + m->width), -real(m->horiBearingY)) / (64.0f)) + morda::Vec2r(real(outline), real(outline));
			g.verts[3] = (morda::Vec2r(real(m->horiBearingX), -real(m->horiBearingY)) / (64.0f)) + morda::Vec2r(-real(outline), real(outline));

			g.texCoords[0] = morda::Vec2r(real(curX), real(curY + im.dim().y));
			g.texCoords[1] = morda::Vec2r(real(curX + im.dim().x), real(curY + im.dim().y));
			g.texCoords[2] = morda::Vec2r(real(curX + im.dim().x), real(curY));
			g.texCoords[3] = morda::Vec2r(real(curX), real(curY));

			//update bounding box if needed
			utki::clampTop(left, g.verts[0].x);
			utki::clampBottom(right, g.verts[2].x);
			utki::clampBottom(bottom, g.verts[0].y);
			utki::clampTop(top, g.verts[2].y);

			ASSERT(-(top - bottom) >= 0) //width >= 0
			ASSERT(right - left >= 0) //height >= 0
			
			auto& r = morda::inst().renderer();
			g.vao = r.factory->createVertexArray(
					{
						r.factory->createVertexBuffer(utki::wrapBuf(g.verts)),
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

		texImg.blit(curX, curY, im);
		curX += im.dim().x + DXGap;
	}//~for
	
	using std::ceil;
	
	this->height_v = ceil((static_cast<FT_Face&>(face)->size->metrics.height) / 64.0f);
	this->descender_v = -ceil((static_cast<FT_Face&>(face)->size->metrics.descender) / 64.0f);
	this->ascender_v = ceil((static_cast<FT_Face&>(face)->size->metrics.ascender) / 64.0f);

	TRACE(<< "TexFont::load(): height_v = " << this->height_v << std::endl)
	
//	TRACE(<< "TexFont::Load(): for loop finished" << std::endl)

	//if there is only one row of characters on the texture image and it does not reach the right edge
	//of the image, we can shrink the image, possibly.
	if(curY == 0){
		//TODO: optimize somehow?
		//resize
		RasterImage copy(texImg);
		texImg.init(kolme::Vec2ui(FindNextPowOf2(curX), copy.dim().y), copy.colorDepth());
		texImg.clear();
		texImg.blit(0, 0, copy);
	}

	//fill luminance channel with 0xff
	if(outline == 0){
		texImg.clear(0, 0xff);
	}

	//now the font image has its final width and heights (no more resizes will be done)

//	for(T_GlyphsIter i = this->glyphs.begin(); i != this->glyphs.end(); ++i){
//		//normalize texture coordinates
//		for(unsigned j = 0; j < i->second.texCoords.size(); ++j){
//			i->second.texCoords[j].compDivBy(texImg.dim().to<float>());
//		}
//		auto& r = morda::inst().renderer();
//		i->second.vao = r.factory->createVertexArray(
//				{
//					r.factory->createVertexBuffer(utki::wrapBuf(i->second.verts)),
//					texCoords//r.factory->createVertexBuffer(utki::wrapBuf(i->second.texCoords))
//				},
//				indexBuffer,
//				VertexArray::Mode_e::TRIANGLE_FAN
//			);
//		i->second.tex = morda::inst().renderer().factory->createTexture2D(
//				morda::numChannelsToTexType(im.numChannels()),
//				im.dim(),
//				im.buf()
//			);
//	}

//	TRACE(<< "TexFont::Load(): initing texture" << std::endl)
	this->tex = morda::inst().renderer().factory->createTexture2D(
			morda::numChannelsToTexType(texImg.numChannels()),
			texImg.dim(),
			texImg.buf()
		);
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
		left = g.verts[0].x;
		right = g.verts[2].x;
		top = g.verts[2].y;
		bottom = g.verts[0].y;
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

		if(g.verts[2].y < top){
			top = g.verts[2].y;
		}

		if(g.verts[0].y > bottom){
			bottom = g.verts[0].y;
		}

		if(curAdvance + g.verts[0].x < left){
			left = curAdvance + g.verts[0].x;
		}

		if(curAdvance + g.verts[2].x > right){
			right = curAdvance + g.verts[2].x;
		}

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
