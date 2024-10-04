/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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
#include "../util/util.hpp"

using namespace ruis;

namespace {
constexpr const char32_t unknown_char = 0xfffd;

constexpr const auto freetype_granularity = 64;
} // namespace

freetype_face::freetype_lib_wrapper::freetype_lib_wrapper()
{
	if (FT_Init_FreeType(&this->lib)) {
		throw std::runtime_error("freetype_lib_wrapper::freetype_lib_wrapper(): unable to init freetype library");
	}
}

freetype_face::freetype_lib_wrapper::~freetype_lib_wrapper()
{
	FT_Done_FreeType(this->lib);
}

freetype_face::freetype_face_wrapper::freetype_face_wrapper(FT_Library& lib, const papki::file& fi)
{
	this->font_file = fi.load();
	ASSERT(!this->font_file.empty())
	if (FT_New_Memory_Face(lib, this->font_file.data(), int(this->font_file.size()), 0 /* face_index */, &this->f) != 0)
	{
		throw std::runtime_error("freetype_face_wrapper::freetype_face_wrapper(): unable to crate font face object");
	}
}

freetype_face::freetype_face_wrapper::~freetype_face_wrapper() noexcept
{
	FT_Done_Face(this->f);
}

freetype_face::freetype_face(const papki::file& fi) :
	face(freetype.lib, fi)
{}

void freetype_face::set_size(unsigned font_size) const
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

freetype_face::metrics freetype_face::get_metrics(unsigned font_size) const
{
	this->set_size(font_size);
	using std::ceil;
	return {
		ceil(real(this->face.f->size->metrics.height) / real(freetype_granularity)), // height
		-ceil(real(this->face.f->size->metrics.descender) / real(freetype_granularity)), // descender
		ceil(real(this->face.f->size->metrics.ascender) / real(freetype_granularity)) // ascender
	};
}

freetype_face::glyph freetype_face::load_glyph(char32_t c, unsigned font_size) const
{
	// set character size in pixels
	this->set_size(font_size);

	if (FT_Load_Char(this->face.f, FT_ULong(c), FT_LOAD_RENDER) != 0) {
		if (c == unknown_char) {
			throw std::runtime_error(
				"texture_font::load_glyph(): could not load 'unknown character' glyph (UTF-32: 0xfffd)"
			);
		}
		LOG([&](auto& o) {
			o << "texture_font::load_glyph(" << std::hex << uint32_t(c) << "): failed to load glyph" << std::endl;
		})
		return {
			{}, // vertices
			{}, // image
			-1 // advance
		};
	}

	FT_GlyphSlot slot = this->face.f->glyph;

	FT_Glyph_Metrics* m = &slot->metrics;

	auto advance = real(m->horiAdvance) / real(freetype_granularity);

	// std::cout << "image advance for char " << c << " = " << advance << std::endl;

	if (!slot->bitmap.buffer) {
		// empty glyph (space)
		return glyph{
			{}, // vertices
			{}, // image
			advance
		};
	}

	ASSERT(slot->format == FT_GLYPH_FORMAT_BITMAP)
	ASSERT(slot->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
	ASSERT(slot->bitmap.pitch >= 0)

	return glyph{
		// vertices
		{
         (ruis::vector2(real(m->horiBearingX), -real(m->horiBearingY)) / real(freetype_granularity)),
         (ruis::vector2(real(m->horiBearingX), real(m->height - m->horiBearingY)) / real(freetype_granularity)),
         (ruis::vector2(real(m->horiBearingX + m->width), real(m->height - m->horiBearingY)) /
			 real(freetype_granularity)),
         (ruis::vector2(real(m->horiBearingX + m->width), -real(m->horiBearingY)) / real(freetype_granularity)) //
		},
		// image
		rasterimage::image<uint8_t, 1>::make(
			{slot->bitmap.width, slot->bitmap.rows},
			slot->bitmap.buffer,
			slot->bitmap.pitch
		),
		advance
	};
}

texture_font::glyph texture_font::load_glyph(char32_t c) const
{
	auto ftg = this->face.get().load_glyph(c, this->font_size);

	if (ftg.advance < 0) {
		return this->unknown_glyph;
	}

	glyph g;
	g.advance = ftg.advance;

	if (ftg.image.empty()) {
		g.top_left.set(0);
		g.bottom_right.set(0);
		// empty glyph (space)
		return g;
	}

	g.top_left = ftg.vertices[0];
	g.bottom_right = ftg.vertices[2];

	auto& r = this->context.get().renderer.get();
	g.vao = r.factory
				->create_vertex_array(
					{r.factory->create_vertex_buffer(utki::make_span(ftg.vertices)),
					 this->context.get().renderer.get().quad_01_vbo},
					this->context.get().renderer.get().quad_indices,
					render::vertex_array::mode::triangle_fan
				)
				.to_shared_ptr();
	g.tex = this->context.get()
				.renderer.get()
				.factory
				->create_texture_2d(
					std::move(ftg.image),
					{.min_filter = render::texture_2d::filter::nearest,
					 .mag_filter = render::texture_2d::filter::nearest,
					 .mipmap = render::texture_2d::mipmap::none}
				)
				.to_shared_ptr();

	return g;
}

texture_font::texture_font(
	const utki::shared_ref<ruis::context>& c,
	// NOLINTNEXTLINE(modernize-pass-by-value)
	const utki::shared_ref<const freetype_face>& face,
	unsigned font_size,
	unsigned max_cached
) :
	font(c),
	font_size(font_size),
	face(face),
	max_cached(max_cached)
{
	//	TRACE(<< "texture_font::Load(): enter" << std::endl)

	this->unknown_glyph = this->load_glyph(unknown_char);

	//	TRACE(<< "texture_font::Load(): entering for loop" << std::endl)

	using std::ceil;

	auto m = this->face.get().get_metrics(this->font_size);

	this->line_height = m.height;
	this->descender = m.descender;
	this->ascender = m.ascender;

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

real texture_font::render_glyph_internal(const ruis::matrix4& matrix, r4::vector4<float> color, char32_t ch) const
{
	const glyph& g = this->get_glyph(ch);

	// texture can be null for glyph of empty characters, like space, tab etc...
	if (g.tex) {
		ASSERT(g.vao)
		this->context.get().renderer.get().shader->color_pos_tex_alpha->render(matrix, *g.vao, color, *g.tex);
	}

	return g.advance;
}

real texture_font::get_advance_internal(std::u32string_view str, unsigned tab_size) const
{
	real ret = 0;

	real space_advance = this->get_glyph(U' ').advance;

	for (auto c : str) {
		try {
			if (c == U'\t') {
				ret += space_advance * real(tab_size);
			} else {
				const glyph& g = this->get_glyph(c);
				ret += g.advance;
			}
		} catch (std::out_of_range&) {
			return ret;
		}
	}

	return ret;
}

ruis::rect texture_font::get_bounding_box_internal(std::u32string_view str, unsigned tab_size) const
{
	ruis::rect ret;

	if (str.empty()) {
		ret.p.set(0);
		ret.d.set(0);
		return ret;
	}

	ASSERT(!str.empty())
	auto s = str.begin();

	// init with bounding box of the first glyph
	auto [cur_advance, left, right, top, bottom] = [&]() {
		const glyph& g = this->get_glyph(*s);
		++s;

		return std::make_tuple(g.advance, g.top_left.x(), g.bottom_right.x(), g.top_left.y(), g.bottom_right.y());
	}();

	real space_advance = this->get_glyph(U' ').advance;

	for (; s != str.end(); ++s) {
		if (*s == U'\t') {
			cur_advance += space_advance * real(tab_size);
		} else {
			const glyph& g = this->get_glyph(*s);

			using std::min;
			using std::max;

			top = min(g.top_left.y(), top);
			bottom = max(g.bottom_right.y(), bottom);
			left = min(cur_advance + g.top_left.x(), left);
			right = max(cur_advance + g.bottom_right.x(), right);

			cur_advance += g.advance;
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
	const ruis::matrix4& matrix,
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

	this->context.get().renderer.get().set_simple_alpha_blending();

	ruis::matrix4 matr(matrix);

	real space_advance = this->get_glyph(U' ').advance;

	size_t cur_offset = offset;

	for (auto c : str) {
		try {
			real advance = [&]() {
				if (c == U'\t') { // if tabulation
					unsigned actual_tab_size = [&]() -> unsigned {
						if (offset == std::numeric_limits<size_t>::max()) {
							return tab_size;
						} else {
							return tab_size - cur_offset % tab_size;
						}
					}();
					auto advance = space_advance * real(actual_tab_size);
					ret.length += actual_tab_size;
					cur_offset += actual_tab_size;
					return advance;
				} else { // all other characters
					auto advance = this->render_glyph_internal(matr, color, c);
					++ret.length;
					++cur_offset;
					return advance;
				}
			}();

			ret.advance += advance;
			matr.translate(advance, 0);
		} catch (std::out_of_range&) {
			return ret;
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
