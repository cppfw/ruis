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

#pragma once

#include <list>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <papki/file.hpp>
#include <r4/rectangle.hpp>
#include <r4/vector.hpp>

#include "../config.hpp"
#include "../render/texture_2d.hpp"
#include "../render/vertex_array.hpp"

#include "font.hpp"

namespace ruis {

class freetype_face
{
	struct freetype_lib_wrapper {
		FT_Library lib = nullptr;

		freetype_lib_wrapper();

		freetype_lib_wrapper(const freetype_lib_wrapper&) = delete;
		freetype_lib_wrapper& operator=(const freetype_lib_wrapper&) = delete;

		freetype_lib_wrapper(freetype_lib_wrapper&&) = delete;
		freetype_lib_wrapper& operator=(freetype_lib_wrapper&&) = delete;

		~freetype_lib_wrapper();
	} freetype;

	struct freetype_face_wrapper {
		FT_Face f = nullptr;
		std::vector<std::uint8_t> font_file; // the buffer should be alive as long as the Face is alive!!!

		freetype_face_wrapper(FT_Library& lib, const papki::file& fi);

		freetype_face_wrapper(const freetype_face_wrapper&) = delete;
		freetype_face_wrapper& operator=(const freetype_face_wrapper&) = delete;

		freetype_face_wrapper(freetype_face_wrapper&&) = delete;
		freetype_face_wrapper& operator=(freetype_face_wrapper&&) = delete;

		~freetype_face_wrapper() noexcept;
	} face;

	void set_size(unsigned font_size) const;

public:
	freetype_face(const papki::file& fi);

	struct glyph {
		std::array<r4::vector2<real>, 4> vertices{};
		rasterimage::image<uint8_t, 1> image;
		real advance = 0;
	};

	glyph load_glyph(char32_t c, unsigned font_size) const;

	struct metrics {
		real height;
		real descender;
		real ascender;
	};

	metrics get_metrics(unsigned font_size) const;
};

/**
 * @brief A texture font.
 * This font implementation reads a Truetype font from 'ttf' file and renders given
 * set of characters to a texture.
 * Then, for rendering strings of text it renders
 * row of quads with texture coordinates corresponding to string characters on the texture.
 */
class texture_font : public font
{
	const unsigned font_size;

	const utki::shared_ref<const freetype_face> face;

	mutable std::list<char32_t> last_used_order;

	struct glyph {
		ruis::vector2 top_left;
		ruis::vector2 bottom_right;

		// TOOD: make utki::shared_ref?
		std::shared_ptr<vertex_array> vao;
		std::shared_ptr<texture_2d> tex;

		real advance = 0;

		decltype(last_used_order)::iterator last_used_iter;
	};

	mutable std::unordered_map<char32_t, glyph> glyphs;

	unsigned max_cached;

	glyph unknown_glyph;

	glyph load_glyph(char32_t c) const;

public:
	/**
	 * @brief Constructor.
	 * @param c - context to which this font belongs.
	 * @param face - freetype font.
	 * @param font_size - size of the font in pixels.
	 * @param max_cached - maximum number of glyphs to cache.
	 */
	texture_font(
		const utki::shared_ref<ruis::context>& c,
		const utki::shared_ref<const freetype_face>& face,
		unsigned font_size,
		unsigned max_cached
	);

	real get_advance(char32_t c, unsigned tab_size) const override;

protected:
	render_result render_internal(
		const ruis::matrix4& matrix,
		r4::vector4<float> color,
		const std::u32string_view str,
		unsigned tab_size,
		size_t offset
	) const override;

	real get_advance_internal(std::u32string_view str, unsigned tab_size) const override;

	ruis::rectangle get_bounding_box_internal(std::u32string_view str, unsigned tab_size) const override;

private:
	real render_glyph_internal(const ruis::matrix4& matrix, r4::vector4<float> color, char32_t ch) const;

	const glyph& get_glyph(char32_t c) const;
};
} // namespace ruis
