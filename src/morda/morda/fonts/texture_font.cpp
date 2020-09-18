#include <algorithm>

#include <utki/debug.hpp>

#include "../util/raster_image.hpp"
#include "../util/util.hpp"

#include "texture_font.hxx"
#include "../context.hpp"

using namespace morda;

namespace{
constexpr const char32_t unknownChar_c = 0xfffd;
}

texture_font::FreeTypeLibWrapper::FreeTypeLibWrapper() {
	if (FT_Init_FreeType(&this->lib)) {
		throw std::runtime_error("FreeTypeLibWrapper::FreeTypeLibWrapper(): unable to init freetype library");
	}
}

texture_font::FreeTypeLibWrapper::~FreeTypeLibWrapper()noexcept{
	FT_Done_FreeType(this->lib);
}

texture_font::FreeTypeFaceWrapper::FreeTypeFaceWrapper(FT_Library& lib, const papki::file& fi) {
	this->fontFile = fi.load();
	if (FT_New_Memory_Face(lib, & * this->fontFile.begin(), int(this->fontFile.size()), 0/* face_index */, &this->f) != 0) {
		throw std::runtime_error("FreeTypeFaceWrapper::FreeTypeFaceWrapper(): unable to crate font face object");
	}
}

texture_font::FreeTypeFaceWrapper::~FreeTypeFaceWrapper()noexcept{
	FT_Done_Face(this->f);
}

texture_font::Glyph texture_font::loadGlyph(char32_t c)const{
	if(FT_Load_Char(this->face.f, FT_ULong(c), FT_LOAD_RENDER) != 0){
		if(c == unknownChar_c){
			throw std::runtime_error("texture_font::loadGlyph(): could not load 'unknown character' glyph (UTF-32: 0xfffd)");
		}
		TRACE(<< "texture_font::loadGlyph(" << std::hex << std::uint32_t(c) << "): failed to load glyph" << std::endl)
		return this->unknownGlyph;
	}
	
	FT_GlyphSlot slot = this->face.f->glyph;
	
	FT_Glyph_Metrics *m = &slot->metrics;
	
	Glyph g;
	g.advance = real(m->horiAdvance) / (64.0f);
	
	if(!slot->bitmap.buffer){
		g.topLeft.set(0);
		g.bottomRight.set(0);
		// empty glyph (space, tab, etc...)
		return g;
	}
	
	raster_image glyphim(r4::vector2<unsigned>(slot->bitmap.width, slot->bitmap.rows), raster_image::color_depth::grey, slot->bitmap.buffer);

	raster_image im(glyphim.dims(), raster_image::color_depth::grey_alpha);
	im.blit({0, 0}, glyphim, 1, 0);
	im.clear(0, std::uint8_t(0xff));
	
	std::array<r4::vector2<float>, 4> verts;
	verts[0] = (morda::vector2(real(m->horiBearingX), -real(m->horiBearingY)) / (64.0f));
	verts[1] = (morda::vector2(real(m->horiBearingX), real(m->height - m->horiBearingY)) / (64.0f));
	verts[2] = (morda::vector2(real(m->horiBearingX + m->width), real(m->height - m->horiBearingY)) / (64.0f));
	verts[3] = (morda::vector2(real(m->horiBearingX + m->width), -real(m->horiBearingY)) / (64.0f));

	g.topLeft = verts[0];
	g.bottomRight = verts[2];

	auto& r = *this->context->renderer;
	g.vao = r.factory->create_vertex_array(
			{
				r.factory->create_vertex_buffer(utki::make_span(verts)),
				this->context->renderer->quad_01_vbo
			},
			this->context->renderer->quad_indices,
			vertex_array::mode::triangle_fan
		);
	g.tex = this->context->renderer->factory->create_texture_2d(
			morda::num_channels_to_texture_type(im.num_channels()),
			im.dims(),
			im.pixels()
		);

	return g;
}

texture_font::texture_font(std::shared_ptr<morda::context> c, const papki::file& fi, unsigned fontSize, unsigned maxCached) :
		font(std::move(c)),
		maxCached(maxCached),
		face(freetype.lib, fi)
{
//	TRACE(<< "texture_font::Load(): enter" << std::endl)

	// set character size in pixels
	{
		FT_Error error = FT_Set_Pixel_Sizes(
				this->face.f,
				0, // pixel_width (0 means "same as height")
				fontSize // pixel_height
			);

		if(error != 0){
			throw std::runtime_error("texture_font::texture_font(): unable to set char size");
		}
	}
	
	this->unknownGlyph = this->loadGlyph(unknownChar_c);

//	TRACE(<< "texture_font::Load(): entering for loop" << std::endl)
	
	using std::ceil;
	
	this->height = ceil((this->face.f->size->metrics.height) / 64.0f);
	this->descender = -ceil((this->face.f->size->metrics.descender) / 64.0f);
	this->ascender = ceil((this->face.f->size->metrics.ascender) / 64.0f);

//	TRACE(<< "texture_font::texture_font(): height_v = " << this->height_v << std::endl)
}

const texture_font::Glyph& texture_font::getGlyph(char32_t c)const{
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
//		TRACE(<< "texture_font::getGlyph(): glyph loaded: " << c << std::endl)
	}else{
		Glyph& g = i->second;
		this->lastUsedOrder.splice(this->lastUsedOrder.begin(), this->lastUsedOrder, g.lastUsedIter);
	}
	
	ASSERT(this->lastUsedOrder.size() <= this->maxCached)
		
	return i->second;
}

real texture_font::renderGlyphInternal(const morda::matrix4& matrix, r4::vector4<float> color, char32_t ch)const{
	const Glyph& g = this->getGlyph(ch);
	
	// texture can be null for glyph of empty characters, like space, tab etc...
	if(g.tex){
		ASSERT(g.vao)
		this->context->renderer->shader->color_pos_tex->render(matrix, *g.vao, color, *g.tex);
	}

	return g.advance;
}

real texture_font::get_advance_internal(const std::u32string& str)const{
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

morda::rectangle texture_font::get_bounding_box_internal(const std::u32string& str)const{
	morda::rectangle ret;

	if(str.empty()){
		ret.p.set(0);
		ret.d.set(0);
		return ret;
	}

	ASSERT(!str.empty())
	auto s = str.begin();

	real curAdvance;

	real left, right, top, bottom;
	// init with bounding box of the first glyph
	{
		const Glyph& g = this->getGlyph(*s);
		left = g.topLeft.x();
		right = g.bottomRight.x();
		top = g.topLeft.y();
		bottom = g.bottomRight.y();
		curAdvance = g.advance;
		++s;
	}

	for(; s != str.end(); ++s){
		const Glyph& g = this->getGlyph(*s);

		using std::min;
		using std::max;
		
		top = min(g.topLeft.y(), top);
		bottom = max(g.bottomRight.y(), bottom);
		left = min(curAdvance + g.topLeft.x(), left);
		right = max(curAdvance + g.bottomRight.x(), right);

		curAdvance += g.advance;
	}

	ret.p.x() = left;
	ret.p.y() = top;
	ret.d.x() = right - left;
	ret.d.y() = -(top - bottom);

	ASSERT(ret.d.x() >= 0)
	ASSERT(ret.d.y() >= 0)
	// TRACE(<< "texture_font::get_bounding_box_internal(): ret = " << ret << std::endl)
	return ret;
}

real texture_font::render_internal(const morda::matrix4& matrix, r4::vector4<float> color, const std::u32string& str)const{
	if(str.size() == 0){
		return 0;
	}
	
	set_simple_alpha_blending(*this->context->renderer);

	real ret = 0;

	morda::matrix4 matr(matrix);

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

real texture_font::get_advance(char32_t c)const{
	auto& g = this->getGlyph(c);
	return g.advance;
}
