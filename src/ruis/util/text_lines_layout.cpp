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

#include "text_lines_layout.hpp"

using namespace ruis;

ruis::text_lines_layout ruis::lay_out_string_into_lines(
	const ruis::font& font, //
	std::u32string_view text,
	real max_width,
	bool multiline,
	bool marquee
)
{
	text_lines_layout res;

	if (text.empty()) {
		return res;
	}

	if (max_width <= 0) {
		// No width constraint yet (e.g. before the first layout): keep the whole text
		// as a single unbroken line without an ellipsis.
		res.lines.push_back(text_line_span{.span = text, .width = font.get_advance(text), .ellipsized = false});
		res.max_line_width = res.lines.back().width;
		return res;
	}

	if (!multiline && marquee) {
		// Single line marquee: keep the whole text on one line at its full width (no
		// truncation), so that the widget can scroll the overflowing part.
		res.lines.push_back(text_line_span{.span = text, .width = font.get_advance(text), .ellipsized = false});
		res.max_line_width = res.lines.back().width;
		return res;
	}

	using std::min;
	using std::max;

	const char32_t space = U' ';
	const real space_advance = font.get_advance(space);
	const real ellipsis_advance = font.get_advance(ellipsis_character);
	// width available for real content on a line that has to end with an ellipsis
	const real avail = min(max_width, max(real(0), max_width - ellipsis_advance));

	// Split the text into words (maximal non-space runs), keeping each word's start index
	// and its advance so that we can build line spans into the original string.
	struct word {
		size_t start;
		size_t len;
		real width;
	};

	std::vector<word> words;

	size_t i = 0;
	const size_t n = text.size();
	while (i < n) {
		while (i < n && text[i] == space) {
			++i; // skip runs of spaces (leading, trailing and between words)
		}
		if (i >= n) {
			break;
		}
		const size_t ws = i;
		while (i < n && text[i] != space) {
			++i;
		}
		const size_t we = i;
		const real w = font.get_advance(std::u32string_view(text.data() + ws, we - ws));
		words.push_back(word{.start = ws, .len = we - ws, .width = w});
	}

	if (words.empty()) {
		return res;
	}

	// Append a line (a span into the text) to the result, recording its width.
	const auto push_line = [&](std::u32string_view span, bool ellip) {
		real w = font.get_advance(span);
		if (ellip) {
			w += ellipsis_advance;
		}
		res.lines.push_back(text_line_span{.span = span, .width = w, .ellipsized = ellip});
		if (w > res.max_line_width) {
			res.max_line_width = w;
		}
	};

	// Truncate a word (given by its position in the text) so that it fits within 'avail'
	// pixels (leaving room for the ellipsis) and return the surviving prefix. At least one
	// character is always kept so that, even when the line is too narrow to fit a character
	// plus the ellipsis, a (possibly clipped) character is still drawn before the ellipsis.
	const auto truncate_word = [&](size_t ws, size_t len) -> std::u32string_view {
		real acc = 0;
		size_t cnt = 0;
		for (size_t c = 0; c != len; ++c) {
			const real ca = font.get_advance(text[ws + c]);
			if (acc + ca > avail) {
				break;
			}
			acc += ca;
			++cnt;
		}
		if (cnt == 0 && len > 0) {
			cnt = 1;
		}
		return {text.data() + ws, cnt};
	};

	size_t cur_first = 0; // index (in words) of the first word on the current line
	real cur_width = 0; // width of the words on the current line (including their spaces)

	for (size_t k = 0; k != words.size(); ++k) {
		const auto& wd = words[k];
		const bool too_long = wd.width > max_width;
		const real added = (k == cur_first) ? wd.width : cur_width + space_advance + wd.width;
		const bool fits = !too_long && (k == 0 || added <= max_width);

		if (fits) {
			cur_width = added;
			continue;
		}

		// The word at index k does not fit on the current line.
		if (multiline) {
			if (too_long) {
				// The word is wider than the whole line: commit the words that already fit on
				// the current line (if any), then put the word on its own line. When not in
				// marquee mode it is truncated to fit and terminated with an ellipsis; in marquee
				// mode it is kept at its full width so that it can be scrolled. The words that
				// follow it continue on subsequent lines.
				if (k > cur_first) {
					push_line(
						std::u32string_view(
							text.data() + words[cur_first].start, //
							words[k - 1].start + words[k - 1].len - words[cur_first].start
						),
						false
					);
				}
				if (marquee) {
					push_line(std::u32string_view(text.data() + wd.start, wd.len), false);
				} else {
					push_line(truncate_word(wd.start, wd.len), true);
				}
				cur_first = k + 1;
				cur_width = 0;
				continue;
			}

			// Move to the next line with the current word.
			push_line(
				std::u32string_view(
					text.data() + words[cur_first].start, //
					words[k - 1].start + words[k - 1].len - words[cur_first].start
				),
				false
			);
			cur_first = k;
			cur_width = wd.width;
		} else {
			// Single line: keep the words that fit so far and terminate them with an ellipsis.
			size_t cs = 0;
			size_t ce = 0;
			if (k > cur_first) {
				cs = words[cur_first].start;
				ce = words[k - 1].start + words[k - 1].len;
			} else {
				// no word fit at all, so the (too long) current word is truncated in place
				cs = wd.start;
				ce = wd.start + wd.len;
			}
			auto acc = std::u32string_view(text.data() + cs, ce - cs);
			if (font.get_advance(acc) <= avail) {
				push_line(acc, true);
			} else {
				// the words that fit still do not leave room for the ellipsis, so truncate them.
				// keep at least one (possibly clipped) character so that the ellipsis is drawn.
				real a2 = 0;
				size_t cnt = 0;
				for (const char32_t ch : acc) {
					const real ca = font.get_advance(ch);
					if (a2 + ca > avail) {
						break;
					}
					a2 += ca;
					++cnt;
				}
				if (cnt == 0) {
					cnt = 1;
				}
				push_line(std::u32string_view(text.data() + cs, cnt), true);
			}
			return res;
		}
	}

	// All words fit, commit the final line without an ellipsis.
	if (cur_first < words.size()) {
		push_line(
			std::u32string_view(
				text.data() + words[cur_first].start, //
				words[words.size() - 1].start + words[words.size() - 1].len - words[cur_first].start
			),
			false
		);
	}

	return res;
}
