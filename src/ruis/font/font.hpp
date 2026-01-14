/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include <string>

#include <r4/matrix.hpp>
#include <r4/rectangle.hpp>
#include <utki/unicode.hpp>

#include "../config.hpp"
#include "../context.hpp"

namespace ruis {

/**
 * @brief Abstract font.
 */
class font
{
protected:
	/**
	 * @brief Distance between baselines.
	 */
	real line_height = 0;

	real descender = 0;

	real ascender = 0;

	font() = default;

public:
	font(const font&) = delete;
	font& operator=(const font&) = delete;

	font(font&&) = delete;
	font& operator=(font&&) = delete;

	/**
	 * @brief String rendering result.
	 */
	struct render_result {
		/**
		 * @brief String advance.
		 */
		real advance;

		/**
		 * @brief String length in characters.
		 * This only makes sence for monospaced fonts.
		 * This takes tabulations into account.
		 */
		size_t length;
	};

protected:
	/**
	 * @brief Render string of text.
	 * @param matrix - transformation matrix to use when rendering the text.
	 * @param color - text color.
	 * @param str - string of text to render.
	 * @param tab_size - tab size in characters. For non-monospace fonts this is the number of space-character advances.
	 * @param offset - in case the string rendered is a sub-string of a bigger string, this is a sub-string offset from
	 * the string start. This is only for monospaced fonts. This is used to calculated proper tab size as it depends on
	 * at which place of the string it appears. Value of std::numeric_limits<size_t>::max() indicates that no tabulator
	 * length adjustement will be done.
	 * @return An advance to the end of the rendered text string. It can be used to position the next text string when
	 * rendering.
	 */
	virtual render_result render_internal(
		render::renderer& renderer, //
		const ruis::matrix4& matrix,
		const ruis::color& color,
		const std::u32string_view str,
		unsigned tab_size,
		size_t offset
	) const = 0;

	/**
	 * @brief Get string advance.
	 * @param str - string of text to get advance for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Advance of the text string.
	 */
	virtual real get_advance_internal(
		std::u32string_view str, //
		unsigned tab_size
	) const = 0;

	/**
	 * @brief Get bounding box of the string.
	 * @param str - string of text to get the bounding box for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Bounding box of the text string.
	 */
	virtual ruis::rect get_bounding_box_internal(
		std::u32string_view str, //
		unsigned tab_size
	) const = 0;

public:
	virtual ~font() = default;

	/**
	 * @brief Render string of text.
	 * @param matrix - transformation matrix to use when rendering.
	 * @param color - text color.
	 * @param str - string of text to render.
	 * @param tab_size - tab size in characters. For non-monospace fonts this is the number of space-character advances.
	 * @param offset - in case the string rendered is a sub-string of a bigger string, this is a sub-string offset from
	 * the string start. This is only for monospaced fonts. This is used to calculated proper tab size as it depends on
	 * at which place of the string it appears.
	 * @return Render result.
	 */
	render_result render(
		render::renderer& renderer, //
		const ruis::matrix4& matrix,
		const ruis::color& color,
		const std::u32string_view str,
		unsigned tab_size = 4,
		size_t offset = std::numeric_limits<size_t>::max()
	) const
	{
		return this->render_internal(
			renderer, //
			matrix,
			color,
			str,
			tab_size,
			offset
		);
	}

	/**
	 * @brief Render string of text.
	 * @param matrix - transformation matrix to use when rendering.
	 * @param color - text color.
	 * @param str - string of text to render.
	 * @param tab_size - tab size in characters. For non-monospace fonts this is the number of space-character advances.
	 * @param offset - in case the string rendered is a sub-string of a bigger string, this is a sub-string offset from
	 * the string start. This is only for monospaced fonts. This is used to calculated proper tab size as it depends on
	 * at which place of the string it appears.
	 * @return Render result.
	 */
	render_result render(
		render::renderer& renderer, //
		const ruis::matrix4& matrix,
		const ruis::color& color,
		utki::utf8_iterator str,
		unsigned tab_size = 4,
		size_t offset = std::numeric_limits<size_t>::max()
	) const
	{
		return this->render(
			renderer, //
			matrix,
			color,
			utki::to_utf32(str),
			tab_size,
			offset
		);
	}

	/**
	 * @brief Render string of text.
	 * @param matrix - transformation matrix to use when rendering.
	 * @param color - text color.
	 * @param str - string of text to render.
	 * @param tab_size - tab size in characters. For non-monospace fonts this is the number of space-character advances.
	 * @param offset - in case the string rendered is a sub-string of a bigger string, this is a sub-string offset from
	 * the string start. This is only for monospaced fonts. This is used to calculated proper tab size as it depends on
	 * at which place of the string it appears.
	 * @return Render result.
	 */
	render_result render(
		render::renderer& renderer, //
		const ruis::matrix4& matrix, //
		const ruis::color& color,
		const char* str,
		unsigned tab_size = 4,
		size_t offset = std::numeric_limits<size_t>::max()
	) const
	{
		return this->render(
			renderer, //
			matrix,
			color,
			utki::utf8_iterator(str),
			tab_size,
			offset
		);
	}

	/**
	 * @brief Render string of text.
	 * @param matrix - transformation matrix to use when rendering.
	 * @param color - text color.
	 * @param str - string of text to render.
	 * @param tab_size - tab size in characters. For non-monospace fonts this is the number of space-character advances.
	 * @param offset - in case the string rendered is a sub-string of a bigger string, this is a sub-string offset from
	 * the string start. This is only for monospaced fonts. This is used to calculated proper tab size as it depends on
	 * at which place of the string it appears.
	 * @return Redner result.
	 */
	render_result render(
		render::renderer& renderer, //
		const ruis::matrix4& matrix, //
		const ruis::color& color,
		std::string_view str,
		unsigned tab_size = 4,
		size_t offset = std::numeric_limits<size_t>::max()
	) const
	{
		return this->render(
			renderer, //
			matrix,
			color,
			utki::utf8_iterator(str),
			tab_size,
			offset
		);
	}

	/**
	 * @brief Get string advance.
	 * @param str - string to get advance for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Advance of the string of text.
	 */
	real get_advance(
		utki::utf8_iterator str, //
		unsigned tab_size = 4
	) const
	{
		return this->get_advance_internal(utki::to_utf32(str), tab_size);
	}

	/**
	 * @brief Get string advance.
	 * @param str - string to get advance for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Advance of the string of text.
	 */
	real get_advance(
		const std::u32string& str, //
		unsigned tab_size = 4
	) const
	{
		return this->get_advance_internal(str, tab_size);
	}

	/**
	 * @brief Get string advance.
	 * @param str - string to get advance for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Advance of the string of text.
	 */
	real get_advance(
		const char* str, //
		unsigned tab_size = 4
	) const
	{
		return this->get_advance(utki::utf8_iterator(str), tab_size);
	}

	/**
	 * @brief Get string advance.
	 * @param str - string to get advance for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Advance of the string of text.
	 */
	real get_advance(
		const std::string& str, //
		unsigned tab_size = 4
	) const
	{
		return this->get_advance(str.c_str(), tab_size);
	}

	/**
	 * @brief Get advance of the character.
	 * @param c - character to get advance for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Advance of the character.
	 */
	virtual real get_advance(
		char32_t c, //
		unsigned tab_size = 4
	) const = 0;

	/**
	 * @brief Get bounding box of the string.
	 * @param str - string of text to get the bounding box for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Bounding box of the text string.
	 */
	ruis::rect get_bounding_box(
		utki::utf8_iterator str, //
		unsigned tab_size = 4
	) const
	{
		return this->get_bounding_box_internal(utki::to_utf32(str), tab_size);
	}

	/**
	 * @brief Get bounding box of the string.
	 * @param str - string of text to get the bounding box for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Bounding box of the text string.
	 */
	ruis::rect get_bounding_box(
		const std::u32string& str, //
		unsigned tab_size = 4
	) const
	{
		return this->get_bounding_box_internal(str, tab_size);
	}

	/**
	 * @brief Get bounding box of the string.
	 * @param str - string of text to get the bounding box for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Bounding box of the text string.
	 */
	ruis::rect get_bounding_box(
		const char* str, //
		unsigned tab_size = 4
	) const
	{
		return this->get_bounding_box(utki::utf8_iterator(str), tab_size);
	}

	/**
	 * @brief Get bounding box of the string.
	 * @param str - string of text to get the bounding box for.
	 * @param tab_size - tabulation size in widths of space character.
	 * @return Bounding box of the text string.
	 */
	ruis::rect get_bounding_box(
		const std::string& str, //
		unsigned tab_size = 4
	) const
	{
		return this->get_bounding_box(str.c_str(), tab_size);
	}

	/**
	 * @brief Baseline to baseline distance.
	 * Disatance between baselines of two consecutive lines of text.
	 */
	real get_line_height() const noexcept
	{
		return this->line_height;
	}

	/**
	 * @brief Get height of the font.
	 * Height of the font is the vertical size of the all glyphs bounding box.
	 * All glyphs bounding box is a bounding box enclosing all the font glyphs as if
	 * they all were drawn at the same origin point.
	 * @return Height of the font.
	 */
	real get_height() const noexcept
	{
		return this->get_ascender() + this->get_descender();
	}

	/**
	 * @brief Distance from the baseline to the bottom edge of all glyphs bounding box.
	 * Positive value.
	 * The assumption is that the baseline intersects the all glyphs bounding box.
	 */
	real get_descender() const noexcept
	{
		return this->descender;
	}

	/**
	 * @brief Distance from the baseline to the top edge of all glyphs bounding box.
	 * Positive value.
	 * The assumption is that the baseline intersects the all glyphs bounding box.
	 */
	real get_ascender() const noexcept
	{
		return this->ascender;
	}
};

} // namespace ruis
