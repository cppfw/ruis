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

#include <functional>

#include "../../../animation/animation.hpp"
#include "../../../paint/rectangle_vao.hpp"
#include "../../../style/styled.hpp"
#include "../../../util/oriented.hpp"
#include "../../../util/timer.hpp"
#include "../../base/fraction_band_widget.hpp"

namespace ruis::touch {

/**
 * @brief Touch scroll bar widget.
 * A non-interactive scroll indicator rendered as two capsules: a background
 * (track) capsule and a handle capsule. The capsules are rectangles with a
 * "very large" equal corner radius (see big_length_px) which is normalized
 * against the widget's dimensions (see normalize_corner_radii()), so the
 * resulting shape is a capsule (stadium). The background track uses the
 * style's 'color_secondary' when defined (and is not drawn when the color is
 * undefined, which is the default) and the handle is another capsule whose
 * position and length follow the widget's fraction and band_fraction values.
 *
 * The widget is a passive indicator: it does not handle any input. Initially it
 * is not drawn at all (the widget itself stays visible, only its drawing is
 * suppressed). Whenever the fraction or the band changes the widget becomes
 * visible (drawn fully opaque) and (re)starts an internal delay timer. After the
 * timer expires the widget fades out linearly (alpha 1 -> 0) over the fade
 * duration, and when the animation ends it becomes not-drawn again. Every
 * fraction or band change resets the delay timer and cancels any in-progress
 * fade.
 *
 * If the thin (transverse) dimension is not set to a concrete value (i.e. it is
 * undefined or 'min') it is defaulted to 7pp.
 */
class scroll_bar :
	public fraction_band_widget, //
	protected oriented
{
public:
	struct specific_parameters {
		/**
		 * @brief The color of the background (track) capsule.
		 * If undefined (the default) then the background is not drawn at all;
		 * set it to a defined color to draw the background track.
		 */
		styled<ruis::color> background_color;

		/**
		 * @brief The color of the handle capsule.
		 * If undefined then the style's 'color_text' is used.
		 */
		styled<ruis::color> handle_color;

		/**
		 * @brief Whether to round the corners of the capsules.
		 * If false the corner radius is 0 (sharp corners, i.e. plain rectangles).
		 */
		bool rounded_corners = true;

		/**
		 * @brief How long (in ms) the indicator stays fully visible after a
		 * fraction/band change before the fade-out begins.
		 */
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "default fade timeout in ms")
		uint32_t fade_timeout_ms = 1000;

		/**
		 * @brief How long (in ms) the fade-out animation (alpha 1 -> 0) lasts.
		 */
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "default fade duration in ms")
		uint32_t fade_duration_ms = 200;
	};

	struct parameters {
		/**
		 * @brief The initial fraction value, from [0:1].
		 */
		real fraction = 0;

		/**
		 * @brief The initial band (handle length) fraction, from [0:1].
		 */
		real band_fraction = 0;

		oriented::parameters orientation;

		specific_parameters specific;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

private:
	enum class state {
		hidden, // not drawn
		waiting, // fully visible, delay timer running
		fading // fading out (alpha 1 -> 0)
	};

	parameters params;

	ruis::paint::rectangle_vao vao;

	// one-shot timer that, once it expires, triggers the fade-out
	utki::shared_ref<ruis::timer> fade_timeout_timer;

	// the fade-out animation (alpha 1 -> 0)
	utki::shared_ref<ruis::animation> fade_animation;

	state cur_state = state::hidden;
	// fade progress of the whole indicator, 0 = fully visible, 1 = fully hidden
	real fade_factor = 0;

	void on_fraction_change() override;
	void on_band_change() override;
	void on_resize() override;

	// called by the delay timer when it expires
	void on_delay_expire();

	// make the indicator visible and (re)start the delay timer
	void on_scroll_indicator_change();

	// current overall alpha (1 = fully opaque, 0 = fully transparent)
	real current_alpha() const;

	// scale the alpha channel of a color by the given factor [0:1]
	static color scale_alpha(const color& c, real f);

public:
	scroll_bar(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);

	scroll_bar(const scroll_bar&) = delete;
	scroll_bar& operator=(const scroll_bar&) = delete;

	scroll_bar(scroll_bar&&) = delete;
	scroll_bar& operator=(scroll_bar&&) = delete;

	~scroll_bar() override;

private:
	void render(const ruis::mat4& matrix) const override;
};

namespace make {
/**
 * @brief Construct a 'touch::scroll_bar' widget.
 * @param context - ruis context.
 * @param params - 'touch::scroll_bar' widget parameters.
 * @return newly constructed 'touch::scroll_bar' widget.
 */
utki::shared_ref<ruis::touch::scroll_bar> scroll_bar(
	const utki::shared_ref<ruis::context>& context, //
	ruis::touch::scroll_bar::all_parameters params
);
} // namespace make

} // namespace ruis::touch
