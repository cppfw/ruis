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

#include "text.hpp"

#include "../../animation/easing.hpp"
#include "../../font/font.hpp"
#include "../../util/text_lines_layout.hpp"
#include "../../util/util.hpp"

using namespace ruis;

text::text(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	string text
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget)
	),
	text_string_widget(
		context, //
		std::move(params.params.text_string),
		std::move(text)
	),
	params([&]() {
		if (auto& a = params.params.specific.align.x(); a.get() == ruis::align::undefined) {
			a = ruis::align::front;
		}
		return std::move(params.params.specific);
	}())
{
	this->recompute_lines();
}

text::~text()
{
	this->context.get().updater.get().stop(*static_cast<updateable*>(this));
}

void text::update(uint32_t dt)
{
	this->marquee_time += dt;
	this->clear_cache();
}

void text::on_lay_out()
{
	this->recompute_lines();
	this->update_marquee_subscription();
}

void text::on_font_change()
{
	this->recompute_lines();
}

void text::recompute_lines()
{
	auto ll = lay_out_string_into_lines(
		this->get_font(), //
		this->get_string().get(), //
		this->rect().d.x(), //
		this->params.multiline, //
		this->params.marquee
	);

	this->lines = std::move(ll.lines);
}

vec2 text::measure(
	const ruis::vec2& quotum, //
	const r4::vector2<measure_mode>& mode
) const noexcept
{
	if (this->params.multiline) {
		// In multiline mode the width determines how the text is wrapped, so break it to
		// lines against the available width to find out the resulting height (and, in
		// at_most mode, the natural width which is the widest line).
		auto ll = lay_out_string_into_lines(
			this->get_font(), //
			this->get_string().get(), //
			quotum.x(), //
			true, //
			this->params.marquee
		);

		vec2 ret(ll.max_line_width, real(ll.lines.size()) * this->get_font().get_line_height());

		for (unsigned i = 0; i != ret.size(); ++i) {
			if (mode[i] == measure_mode::exactly) {
				ret[i] = quotum[i];
			} else {
				using std::min;
				ret[i] = min(ret[i], quotum[i]);
			}
		}

		return ret;
	}

	return this->text_string_widget::measure(quotum, mode);
}

void text::update_marquee_subscription()
{
	if (this->is_marquee()) {
		// Clip so that the scrolling text does not overflow the widget's rectangle.
		this->set_clip(true);

		if (!this->is_updating()) {
			this->context.get().updater.get().start(utki::make_shared_from(*static_cast<updateable*>(this)));
		}
	} else if (this->is_updating()) {
		this->context.get().updater.get().stop(*static_cast<updateable*>(this));
	}
}

void text::render(const ruis::mat4& matrix) const
{
	const auto& font = this->get_font();
	const auto& dims = this->rect().d;

	// Vertical alignment of the block of lines within the widget's height. The text block
	// is as tall as the number of lines times the line height. For the back (bottom)
	// alignment the last line is aligned to the widget's bottom edge, so when the text
	// block is taller than the widget the offset is negative and the first lines extend
	// beyond the widget's top edge.
	const real text_height_px = real(this->lines.size()) * font.get_line_height();
	real vertical_offset = 0;
	switch (this->params.align.y().get()) {
		case ruis::align::front:
			vertical_offset = 0;
			break;
		case ruis::align::undefined:
			[[fallthrough]];
		case ruis::align::center:
			vertical_offset = (dims.y() - text_height_px) / 2;
			break;
		case ruis::align::back:
			vertical_offset = dims.y() - text_height_px;
			break;
	}

	// The marquee scroll speed in the widget's coordinate unit (pixels) per second. The
	// spec value is in pp/sec, converted to pixels using the context's units.
	const real marquee_speed = real(marquee_speed_pps) * this->ctx().units.dots_per_pp();

	// TODONEXT: use utki::views::enumerate and range based for loop
	for (size_t i = 0; i != this->lines.size(); ++i) {
		const auto& line = this->lines[i];

		// Horizontal position of the line within the widget's width. A line that is wider
		// than the widget and has marquee on is positioned by the current marquee scroll
		// offset; all other lines are positioned by the static horizontal alignment offset.
		real x = 0;
		if (this->is_marquee() && line.width > dims.x() && marquee_speed > 0) {
			// Scroll the line back and forth. The scroll moves with an eased (in-out
			// quadratic) motion, so a wider overflow takes longer to scroll. At each end the
			// marquee stays still for marquee_pause_ms before reversing.
			const real max_off = line.width - dims.x();
			const real leg = max_off / marquee_speed; // one direction, seconds
			const real pause = real(marquee_pause_ms) / real(1000); // pause at each end, seconds
			const real cycle = 2 * (leg + pause); // full back-and-forth including pauses
			const real t = real(this->marquee_time) / real(1000); // elapsed time, seconds
			const real tt = std::fmod(t, cycle); // position within the current cycle, seconds
			real tri = 0;
			if (tt < leg) {
				tri = ruis::easing::in_out_quadratic(tt / leg); // forward leg, eased: 0 -> 1
			} else if (tt < leg + pause) {
				tri = 1; // pause at the far end
			} else if (tt < 2 * leg + pause) {
				const real p = (tt - (leg + pause)) / leg; // 0 -> 1 across the reverse leg
				tri = 1 - ruis::easing::in_out_quadratic(p); // reverse leg, eased: 1 -> 0
			} else {
				tri = 0; // pause at the start
			}
			// A scrolling line spans the whole widget, so the static alignment offset does
			// not apply. The marquee always starts from the beginning of the line.
			x = -max_off * tri;
		} else {
			switch (this->params.align.x().get()) {
				case ruis::align::front:
					break;
				case ruis::align::undefined:
					[[fallthrough]];
				case ruis::align::center:
					x = (dims.x() - line.width) / 2;
					break;
				case ruis::align::back:
					x = dims.x() - line.width;
					break;
			}
		}

		// The final offset is rounded to integer pixels so that the glyphs land on whole
		// pixel positions: a fractional position would be rendered blurred.
		const ruis::vec2 offset = round(ruis::vec2(
			x - font.get_bounding_box(line.span).p.x(), //
			vertical_offset + font.get_ascender() + real(i) * font.get_line_height()
		));

		ruis::mat4 matr(matrix);
		matr.translate(offset.x(), offset.y());
		font.render(
			this->ctx().ren(), //
			matr, //
			this->get_color(), //
			line.span
		);

		// Draw the ellipsis right after a truncated line.
		if (line.ellipsized && line.span.size() != 0) {
			const real adv = font.get_advance(line.span);
			ruis::mat4 em(matrix);
			em.translate(offset.x() + adv, offset.y());
			font.render(
				this->ctx().ren(), //
				em, //
				this->get_color(), //
				std::u32string_view(&ellipsis_character, 1)
			);
		}
	}
}

namespace ruis::make {

utki::shared_ref<ruis::text> text(
	const utki::shared_ref<ruis::context>& context, //
	text::all_parameters params,
	string text
)
{
	return utki::make_shared<ruis::text>(
		context, //
		std::move(params),
		std::move(text)
	);
}

} // namespace ruis::make
