#include <algorithm>


#include <utki/debug.hpp>

#include "../util/RasterImage.hpp"
#include "../util/util.hpp"

#include "TexFont.hxx"
#include "../context.hpp"



using namespace morda;



namespace{

constexpr const char32_t unknownChar_c = 0xfffd;

}

TexFont::FreeTypeLibWrapper::FreeTypeLibWrapper() {
	if (FT_Init_FreeType(&this->lib)) {
		throw utki::Exc("FreeTypeLibWrapper::FreeTypeLibWrapper(): unable to init freetype library");
	}
}

TexFont::FreeTypeLibWrapper::~FreeTypeLibWrapper()noexcept{
	FT_Done_FreeType(this->lib);
}

TexFont::FreeTypeFaceWrapper::FreeTypeFaceWrapper(FT_Library& lib, const papki::File& fi) {
	this->fontFile = fi.loadWholeFileIntoMemory();
	if (FT_New_Memory_Face(lib, & * this->fontFile.begin(), int(this->fontFile.size()), 0/* face_index */, &this->f) != 0) {
		throw utki::Exc("FreeTypeFaceWrapper::FreeTypeFaceWrapper(): unable to crate font face object");
	}
}

TexFont::FreeTypeFaceWrapper::~FreeTypeFaceWrapper()noexcept{
	FT_Done_Face(this->f);
}
TexFont::Glyph TexFont::loadGlyph(char32_t c) const{
	if(FT_Load_Char(this->face.f, FT_ULong(c), FT_LOAD_RENDER) != 0){
		if(c == unknownChar_c){
			throw morda::Exc("TexFont::loadGlyph(): could not load 'unknown character' glyph (UTF-32: 0xfffd)");
		}
		TRACE(<< "TexFont::loadGlyph(" << std::hex << std::uint32_t(c) << "): failed to load glyph" << std::endl)
		return this->unknownGlyph;
	}
	
	FT_GlyphSlot slot = this->face.f->glyph;
	
	FT_Glyph_Metrics *m = &slot->metrics;
	
	Glyph g;
	g.advance = real(m->horiAdvance) / (64.0f);
	
	if(!slot->bitmap.buffer){
		//empty glyph (space, tab, etc...)
		
		return g;
	}
	
	RasterImage glyphim(r4::vec2ui(slot->bitmap.width, slot->bitmap.rows), RasterImage::ColorDepth_e::GREY, slot->bitmap.buffer);

	RasterImage im(glyphim.dim(), RasterImage::ColorDepth_e::GREYA);
	im.blit(0, 0, glyphim, 1, 0);
	im.clear(0, std::uint8_t(0xff));
	
	
	
	std::array<r4::vec2f, 4> verts;
	verts[0] = (morda::Vec2r(real(m->horiBearingX), -real(m->horiBearingY)) / (64.0f));
	verts[1] = (morda::Vec2r(real(m->horiBearingX), real(m->height - m->horiBearingY)) / (64.0f));
	verts[2] = (morda::Vec2r(real(m->horiBearingX + m->width), real(m->height - m->horiBearingY)) / (64.0f));
	verts[3] = (morda::Vec2r(real(m->horiBearingX + m->width), -real(m->horiBearingY)) / (64.0f));

	g.topLeft = verts[0];
	g.bottomRight = verts[2];

	auto& r = *morda::inst().renderer;
	g.vao = r.factory->createVertexArray(
			{
				r.factory->createVertexBuffer(utki::wrapBuf(verts)),
				morda::inst().renderer->quad01VBO
			},
			morda::inst().renderer->quadIndices,
			VertexArray::Mode_e::TRIANGLE_FAN
		);
	g.tex = morda::inst().renderer->factory->createTexture2D(
			morda::numChannelsToTexType(im.numChannels()),
			im.dim(),
			im.buf()
		);
	
	return g;
}


TexFont::TexFont(const papki::file& fi, unsigned fontSize, unsigned maxCached) :
		maxCached(maxCached),
		face(freetype.lib, fi)
{
//	TRACE(<< "TexFont::Load(): enter" << std::endl)

	// set character size in pixels
	{
		FT_Error error = FT_Set_Pixel_Sizes(
				this->face.f,
				0, // pixel_width (0 means "same as height")
				fontSize // pixel_height
			);

		if(error != 0){
			throw utki::Exc("TexFont::TexFont(): unable to set char size");
		}
	}
	
	this->unknownGlyph = this->loadGlyph(unknownChar_c);

//	TRACE(<< "TexFont::Load(): entering for loop" << std::endl)
	
	using std::ceil;
	
	this->height_v = ceil((this->face.f->size->metrics.height) / 64.0f);
	this->descender_v = -ceil((this->face.f->size->metrics.descender) / 64.0f);
	this->ascender_v = ceil((this->face.f->size->metrics.ascender) / 64.0f);

//	TRACE(<< "TexFont::TexFont(): height_v = " << this->height_v << std::endl)
}


const TexFont::Glyph& TexFont::getGlyph(char32_t c)const{
	auto i = this->glyphs.find(c);
	if(i == this->glyphs.end()){
		auto r = this->glyphs.insert(std::make_pair(c, this->loadGlyph(c)));
		ASSERT(r.second)
		i = r.first;
		this->lastUsedOrder.push_front(c);
		i->second.lastUsedIter = this->lastUsedOrder.begin();
		
		if(this->lastUsedOrder.size() == this->maxCached){
			this->glyphs.erase(this->lastUsedOrder.back());
			this->lastUsedOrder.pop_back();
		}
//		TRACE(<< "TexFont::getGlyph(): glyph loaded: " << c << std::endl)
	}else{
		Glyph& g = i->second;
		this->lastUsedOrder.splice(this->lastUsedOrder.begin(), this->lastUsedOrder, g.lastUsedIter);
	}
	
	ASSERT(this->lastUsedOrder.size() <= this->maxCached)
		
	return i->second;
}



real TexFont::renderGlyphInternal(const morda::Matr4r& matrix, r4::vec4f color, char32_t ch)const{
	const Glyph& g = this->getGlyph(ch);
	
	// texture can be null for glyph of empty characters, like space, tab etc...
	if(g.tex){
		ASSERT(g.vao)
		morda::inst().renderer->shader->colorPosTex->render(matrix, *g.vao, color, *g.tex);
	}

	return g.advance;
}



real TexFont::stringAdvanceInternal(const std::u32string& str)const{
	real ret = 0;

	auto s = str.begin();
	
	for(; s != str.end(); ++s){
		try{
			const Glyph& g = this->getGlyph(*s);
			ret += g.advance;
		}catch(std::out_of_range&){
			// ignore
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
		const Glyph& g = this->getGlyph(*s);
		left = g.topLeft.x;
		right = g.bottomRight.x;
		top = g.topLeft.y;
		bottom = g.bottomRight.y;
		curAdvance = g.advance;
		++s;
	}

	for(; s != str.end(); ++s){
		const Glyph& g = this->getGlyph(*s);

		using std::min;
		using std::max;
		
		top = min(g.topLeft.y, top);
		bottom = max(g.bottomRight.y, bottom);
		left = min(curAdvance + g.topLeft.x, left);
		right = max(curAdvance + g.bottomRight.x, right);

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



real TexFont::renderStringInternal(const morda::Matr4r& matrix, r4::vec4f color, const std::u32string& str)const{
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
	auto& g = this->getGlyph(c);
	return g.advance;
}
