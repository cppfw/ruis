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

#include <string_view>
#include <vector>

#include "../config.hpp"
#include "../font/font.hpp"

namespace ruis {

// The ellipsis character used to mark truncated text.
constexpr char32_t ellipsis_character = U'…'; // …

/**
 * @brief A single laid-out line of a text string.
 * Holds the span of the source text that belongs to the line, the line's width (in
 * pixels) and whether the line is truncated (i.e. terminated with a trailing ellipsis).
 */
struct text_line_span {
	std::u32string_view span;
	real width = 0;
	bool ellipsized = false;
};

/**
 * @brief The result of laying out a text string into lines.
 * Holds the resulting lines and the width (in pixels) of the widest line.
 */
struct text_lines_layout {
	std::vector<text_line_span> lines;
	real max_line_width = 0;
};

/**
 * @brief Lay a text string out into lines that fit within a given width.
 * Words (maximal runs of non-space characters) are placed on a line until the next word
 * no longer fits. In multiline mode a word that does not fit is moved to the next line.
 * In single line mode the words that fit are kept and the rest is discarded. A word wider
 * than the whole width cannot be placed as-is, so it is truncated to fit and terminated
 * with an ellipsis. In marquee mode truncation is suppressed: overflowing text is kept at
 * its full width so that the widget can scroll it, and a too-long word in multiline mode is
 * placed on its own (overlapping) line instead of being truncated. In either mode a too-long
 * word never stops the layout: the words that follow it continue on subsequent lines.
 * @param font - font to measure the text with.
 * @param text - text to lay out into lines.
 * @param max_width - maximum line width (in pixels) to wrap to.
 * @param multiline - whether to wrap to multiple lines (true) or keep a single line (false).
 * @param marquee - whether the text is going to be scrolled (marquee): when true, text that
 *        would otherwise be truncated with an ellipsis is instead kept at its full width.
 * @return The resulting layout with the lines and the width of the widest line.
 */
text_lines_layout lay_out_string_into_lines(
	const ruis::font& font, //
	std::u32string_view text,
	real max_width,
	bool multiline,
	bool marquee = false
);

} // namespace ruis
