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

#include <algorithm>
#include <cstdint>
#include <string_view>
#include <vector>

#include "../../res/font.hpp"
#include "../../updateable.hpp"
#include "../../util/text_lines_layout.hpp"
#include "../base/text_string_widget.hpp"
#include "../widget.hpp"

namespace ruis {

/**
 * @brief Text label widget.
 * This widget shows text. The text can be drawn on a single line (the default) or wrapped to
 * multiple lines (see specific_parameters::multiline). Text that overflows the widget's width
 * is either truncated with an ellipsis or scrolled back and forth when marquee mode is enabled
 * (see specific_parameters::marquee).
 * From GUI script it can be instantiated as "text".
 */
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via text_string_widget")
class text :
	public text_string_widget, //
	// The marquee scrolling is a continuous, infinite back-and-forth animation whose cycle
	// duration depends on each line's overflow width and which pauses at each end. The line
	// width (and hence the scroll duration) is known at layout time, so one could in principle
	// create an ruis::animation object for a cycle and recreate it on each (re)layout. However,
	// the marquee repeats indefinitely with a (per-line) variable period and end pauses, which
	// the implemented ruis::animation (one-shot, fixed duration) cannot express directly, and the
	// ruis::repeating_animation / ruis::bouncing_animation that would fit are not implemented.
	// So we subscribe to periodic updates (via ruis::updateable) and derive each line's scroll
	// offset from the total elapsed time; on (re)layout the lines (and hence the offsets) are
	// recomputed.
	// TODO: use bouncing_animation when it is implemented?
	private updateable
{
public:
	struct specific_parameters {
		/**
		 * @brief Whether the text is drawn in multiline mode.
		 * In multiline mode the text is wrapped to the widget's width: words are placed on a
		 * line until a word no longer fits, in which case it is moved to the next line. Words
		 * are not broken, except a single word wider than the whole line, which is truncated
		 * with a trailing ellipsis. When false (the default) the text is drawn on a single line.
		 */
		bool multiline = false;

		/**
		 * @brief Whether text overflowing the widget's width is scrolled back and forth
		 * (marquee) instead of being truncated with an ellipsis.
		 * In single line mode the single line is scrolled when it is wider than the widget.
		 * In multiline mode each line wider than the widget is scrolled independently.
		 * Defaults to false.
		 */
		bool marquee = false;
	};

	struct parameters {
		// The common text string widget parameters (color, font, selection color, style).
		text_string_widget::parameters text_string;
		// The label-specific parameters (multiline, marquee).
		specific_parameters specific;
	};

	struct all_parameters {
		// The qualified names are resolved from the global scope: a plain `ruis::...` lookup
		// inside this nested struct is ambiguous with the enclosing class in this translation unit.
		::ruis::layout_parameters layout_params;
		::ruis::widget::parameters widget;
		parameters params;
	};

	text(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params,
		string text
	);

public:
	text(const text&) = delete;
	text& operator=(const text&) = delete;

	text(text&&) = delete;
	text& operator=(text&&) = delete;

	~text() override;

	void render(const ruis::mat4& matrix) const override;

	vec2 measure(
		const ruis::vec2& quotum, //
		const r4::vector2<measure_mode>& mode
	) const noexcept override;

	/**
	 * @brief Get the text broken into lines (see the 'lines' member).
	 * The lines are recomputed on (re)layout, so their widths are consistent with the
	 * widget's current width. In single line mode there is at most one line; in multiline
	 * mode there is one line per wrapped line.
	 * @return Lines of the text.
	 */
	const std::vector<text_line_span>& get_lines() const noexcept
	{
		return this->lines;
	}

	/**
	 * @brief Check whether the text is in multiline mode.
	 * @return true if multiline mode is enabled.
	 */
	bool is_multiline() const noexcept
	{
		return this->specific_params.multiline;
	}

	/**
	 * @brief Check whether marquee (scrolling of overflowing text) is enabled.
	 * @return true if marquee mode is enabled.
	 */
	bool is_marquee() const noexcept
	{
		return this->specific_params.marquee;
	}

	/**
	 * @brief Check whether any line is truncated and must be drawn with a trailing ellipsis.
	 * @return true if any line needs a trailing ellipsis.
	 * @return false otherwise.
	 */
	bool is_ellipsized() const noexcept
	{
		return std::ranges::any_of(this->lines, [](const text_line_span& l) {
			return l.ellipsized;
		});
	}

private:
	// The text laid out into lines. Each element is a text_line_span holding the span of the
	// text that belongs to the line (without the line separator), its width and whether it is
	// truncated. In single line mode there is at most one element; in multiline mode there is
	// one element per line. A word wider than the widget's width is placed on its own line,
	// stored truncated and marked as ellipsized; the words that follow it continue on
	// subsequent lines.
	std::vector<text_line_span> lines;

	specific_parameters specific_params;

	// Total elapsed time (in milliseconds) spent in marquee mode, used to derive the current
	// marquee phase. Only advanced while the widget is subscribed for updates.
	uint32_t marquee_time = 0;

	// Marquee scrolling speed: the rate at which the marquee scrolls, in perception pixels
	// (pp) per second. The duration of a scroll depends on how much text overflows the widget,
	// so a wider (longer) line takes proportionally longer to scroll back and forth.
	constexpr static auto marquee_speed_pps = 50;

	// Marquee pause: the time (in milliseconds) the marquee stays still at each end before
	// reversing direction.
	constexpr static auto marquee_pause_ms = 1000;

	void on_lay_out() override;

	void on_font_change() override;

	// Advance the marquee animation by the given time and request a re-render.
	void update(uint32_t dt) override;

	// Recompute the lines for the current text and widget width.
	void recompute_lines();

	// Start (or stop) the periodic updates needed to animate the marquee scrolling.
	void update_marquee_subscription();
};

namespace make {
utki::shared_ref<ruis::text> text(
	const utki::shared_ref<ruis::context>& context, //
	text::all_parameters params,
	string text = {}
);
} // namespace make

} // namespace ruis
