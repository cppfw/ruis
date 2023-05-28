/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "texture_font.hxx"

#include <algorithm>

#include <utki/debug.hpp>

#include "../context.hpp"
#include "../util/raster_image.hpp"
#include "../util/util.hpp"

using namespace morda;

namespace {
constexpr const char32_t unknown_char = 0xfffd;
} // namespace

texture_font::freetype_lib_wrapper::freetype_lib_wrapper()
{
	if (FT_Init_FreeType(&this->lib)) {
		throw std::runtime_error("freetype_lib_wrapper::freetype_lib_wrapper(): unable to init freetype library");
	}
}

texture_font::freetype_lib_wrapper::~freetype_lib_wrapper() noexcept
{
	FT_Done_FreeType(this->lib);
}

texture_font::freetype_face_wrapper::freetype_face_wrapper(FT_Library& lib, const papki::file& fi)
{
	this->fontFile = fi.load();
	if (FT_New_Memory_Face(lib, &*this->fontFile.begin(), int(this->fontFile.size()), 0 /* face_index */, &this->f)
		!= 0)
	{
		throw std::runtime_error("freetype_face_wrapper::freetype_face_wrapper(): unable to crate font face object");
	}
}

texture_font::freetype_face_wrapper::~freetype_face_wrapper() noexcept
{
	FT_Done_Face(this->f);
}

texture_font::glyph texture_font::load_glyph(char32_t c) const
{
	if (FT_Load_Char(this->face.f, FT_ULong(c), FT_LOAD_RENDER) != 0) {
		if (c == unknown_char) {
			throw std::runtime_error(
				"texture_font::load_glyph(): could not load 'unknown character' glyph (UTF-32: 0xfffd)"
			);
		}
		LOG([&](auto& o) {
			o << "texture_font::load_glyph(" << std::hex << uint32_t(c) << "): failed to load glyph" << std::endl;
		})
		return this->unknown_glyph;
	}

	FT_GlyphSlot slot = this->face.f->glyph;

	FT_Glyph_Metrics* m = &slot->metrics;

	glyph g;
	g.advance = real(m->horiAdvance) / (64.0f);

	if (!slot->bitmap.buffer) {
		g.top_left.set(0);
		g.bottom_right.set(0);
		// empty glyph (space)
		return g;
	}

	raster_image glyphim(
		r4::vector2<unsigned>(slot->bitmap.width, slot->bitmap.rows),
		raster_image::color_depth::grey,
		slot->bitmap.buffer
	);

	std::array<r4::vector2<float>, 4> verts;
	verts[0] = (morda::vector2(real(m->horiBearingX), -real(m->horiBearingY)) / (64.0f));
	verts[1] = (morda::vector2(real(m->horiBearingX), real(m->height - m->horiBearingY)) / (64.0f));
	verts[2] = (morda::vector2(real(m->horiBearingX + m->width), real(m->height - m->horiBearingY)) / (64.0f));
	verts[3] = (morda::vector2(real(m->horiBearingX + m->width), -real(m->horiBearingY)) / (64.0f));

	g.top_left = verts[0];
	g.bottom_right = verts[2];

	auto& r = this->context.get().renderer.get();
	g.vao = r.factory
				->create_vertex_array(
					{r.factory->create_vertex_buffer(utki::make_span(verts)),
					 this->context.get().renderer.get().quad_01_vbo},
					this->context.get().renderer.get().quad_indices,
					vertex_array::mode::triangle_fan
				)
				.to_shared_ptr();
	g.tex = this->context.get()
				.renderer.get()
				.factory
				->create_texture_2d(
					morda::num_channels_to_texture_type(glyphim.num_channels()),
					glyphim.dims(),
					glyphim.pixels()
				)
				.to_shared_ptr();

	return g;
}

texture_font::texture_font(
	const utki::shared_ref<morda::context>& c,
	const papki::file& fi,
	unsigned font_size,
	unsigned max_cached
) :
	font(c),
	max_cached(max_cached),
	face(freetype.lib, fi)
{
	//	TRACE(<< "texture_font::Load(): enter" << std::endl)

	// set character size in pixels
	{
		FT_Error error = FT_Set_Pixel_Sizes(
			this->face.f,
			0, // pixel_width (0 means "same as height")
			font_size // pixel_height
		);

		if (error != 0) {
			throw std::runtime_error("texture_font::texture_font(): unable to set char size");
		}
	}

	this->unknown_glyph = this->load_glyph(unknown_char);

	//	TRACE(<< "texture_font::Load(): entering for loop" << std::endl)

	using std::ceil;

	this->height = ceil((this->face.f->size->metrics.height) / 64.0f);
	this->descender = -ceil((this->face.f->size->metrics.descender) / 64.0f);
	this->ascender = ceil((this->face.f->size->metrics.ascender) / 64.0f);

	//	TRACE(<< "texture_font::texture_font(): height_v = " << this->height_v << std::endl)
}

const texture_font::glyph& texture_font::get_glyph(char32_t c) const
{
	auto i = this->glyphs.find(c);
	if (i == this->glyphs.end()) {
		auto r = this->glyphs.insert(std::make_pair(c, this->load_glyph(c)));
		ASSERT(r.second)
		i = r.first;
		this->last_used_order.push_front(c);
		i->second.last_used_iter = this->last_used_order.begin();

		if (this->last_used_order.size() == this->max_cached) {
			this->glyphs.erase(this->last_used_order.back());
			this->last_used_order.pop_back();
		}
		//		TRACE(<< "texture_font::get_glyph(): glyph loaded: " << c << std::endl)
	} else {
		glyph& g = i->second;
		this->last_used_order.splice(this->last_used_order.begin(), this->last_used_order, g.last_used_iter);
	}

	ASSERT(this->last_used_order.size() <= this->max_cached)

	return i->second;
}

real texture_font::render_glyph_internal(const morda::matrix4& matrix, r4::vector4<float> color, char32_t ch) const
{
	const glyph& g = this->get_glyph(ch);

	// texture can be null for glyph of empty characters, like space, tab etc...
	if (g.tex) {
		ASSERT(g.vao)
		this->context.get().renderer.get().shader->color_pos_tex_alpha->render(matrix, *g.vao, color, *g.tex);
	}

	return g.advance;
}

real texture_font::get_advance_internal(const std::u32string& str, unsigned tab_size) const
{
	real ret = 0;

	real space_advance = this->get_glyph(U' ').advance;

	for (auto s = str.begin(); s != str.end(); ++s) {
		try {
			if (*s == U'\t') {
				ret += space_advance * tab_size;
			} else {
				const glyph& g = this->get_glyph(*s);
				ret += g.advance;
			}
		} catch (std::out_of_range&) {
			// ignore
		}
	}

	return ret;
}

morda::rectangle texture_font::get_bounding_box_internal(const std::u32string& str, unsigned tab_size) const
{
	morda::rectangle ret;

	if (str.empty()) {
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
		const glyph& g = this->get_glyph(*s);
		left = g.top_left.x();
		right = g.bottom_right.x();
		top = g.top_left.y();
		bottom = g.bottom_right.y();
		curAdvance = g.advance;
		++s;
	}

	real space_advance = this->get_glyph(U' ').advance;

	for (; s != str.end(); ++s) {
		if (*s == U'\t') {
			curAdvance += space_advance * tab_size;
		} else {
			const glyph& g = this->get_glyph(*s);

			using std::min;
			using std::max;

			top = min(g.top_left.y(), top);
			bottom = max(g.bottom_right.y(), bottom);
			left = min(curAdvance + g.top_left.x(), left);
			right = max(curAdvance + g.bottom_right.x(), right);

			curAdvance += g.advance;
		}
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

font::render_result texture_font::render_internal(
	const morda::matrix4& matrix,
	r4::vector4<float> color,
	const std::u32string_view str,
	unsigned tab_size,
	size_t offset
) const
{
	render_result ret = {0, 0};

	if (str.size() == 0) {
		return ret;
	}

	set_simple_alpha_blending(this->context.get().renderer.get());

	morda::matrix4 matr(matrix);

	real space_advance = this->get_glyph(U' ').advance;

	size_t cur_offset = offset;

	for (auto s = str.begin(); s != str.end(); ++s) {
		try {
			real advance;

			if (*s == U'\t') { // if tabulation
				unsigned actual_tab_size;
				if (offset == std::numeric_limits<size_t>::max()) {
					actual_tab_size = tab_size;
				} else {
					actual_tab_size = tab_size - cur_offset % tab_size;
				}
				advance = space_advance * actual_tab_size;
				ret.length += actual_tab_size;
				cur_offset += actual_tab_size;
			} else { // all other characters
				advance = this->render_glyph_internal(matr, color, *s);
				++ret.length;
				++cur_offset;
			}

			ret.advance += advance;
			matr.translate(advance, 0);
		} catch (std::out_of_range&) {
			// ignore
		}
	}

	return ret;
}

real texture_font::get_advance(char32_t c, unsigned tab_size) const
{
	if (c == U'\t') {
		return this->get_glyph(U' ').advance * real(tab_size);
	} else {
		auto& g = this->get_glyph(c);
		return g.advance;
	}
}
