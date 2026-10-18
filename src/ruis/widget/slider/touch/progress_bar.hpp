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

#include "../../../paint/capsule_vao.hpp"
#include "../../../style/styled.hpp"
#include "../../base/fraction_widget.hpp"

namespace ruis::touch {

/**
 * @brief Progress bar widget.
 * A horizontal progress bar rendered as a capsule. The background (track) is a
 * capsule of the style's secondary color and the actual progress is a capsule of
 * the style's special color which grows from the left (0% = just a circle) to the
 * right (100% = the whole bar) following the widget's fraction value.
 * When the layout parameters' vertical dimension is not set to a concrete value
 * (i.e. it is undefined or 'min') it is defaulted to the style's 'len_gap_small'.
 */
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via fraction_widget")
class progress_bar : public fraction_widget
{
public:
	struct specific_parameters {
		/**
		 * @brief The color of the background (track) capsule.
		 * If undefined then the style's 'color_secondary' is used.
		 */
		styled<ruis::color> background_color;

		/**
		 * @brief The color of the progress (bar) capsule.
		 * If undefined then the style's 'color_special' is used.
		 */
		styled<ruis::color> bar_color;
	};

	struct parameters {
		/**
		 * @brief The initial fraction value, from [0:1].
		 */
		real fraction = 0;

		specific_parameters specific;
	};

private:
	parameters params;

	// a single vao is enough for both the background (track) and the bar:
	// the vao's thickness (diameter) is the same for both and the length is
	// passed in at render time
	ruis::paint::capsule_vao vao;

	void on_fraction_change() override;
	void on_resize() override;

public:
	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

	progress_bar(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);

	progress_bar(const progress_bar&) = delete;
	progress_bar& operator=(const progress_bar&) = delete;

	progress_bar(progress_bar&&) = delete;
	progress_bar& operator=(progress_bar&&) = delete;

	~progress_bar() override = default;

	/**
	 * @brief Set the color of the bar (the filled portion) and re-render the widget.
	 * @param c - the new bar color.
	 */
	void set_bar_color(const styled<ruis::color>& c)
	{
		this->params.specific.bar_color = c;
		this->clear_cache();
	}

private:
	void render(const ruis::mat4& matrix) const override;
};

namespace make {
/**
 * @brief Construct a 'progress_bar' widget.
 * @param context - ruis context.
 * @param params - 'progress_bar' widget parameters.
 * @return newly constructed 'progress_bar' widget.
 */
utki::shared_ref<ruis::touch::progress_bar> progress_bar(
	const utki::shared_ref<ruis::context>& context, //
	ruis::touch::progress_bar::all_parameters params
);
} // namespace make

} // namespace ruis::touch
