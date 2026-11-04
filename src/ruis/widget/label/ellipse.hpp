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

#include "../../paint/ellipse_vao.hpp"

#include "padding.hpp"

namespace ruis {

/**
 * @brief Ellipse widget.
 * This is a widget which can display a filled ellipse and/or a stroked ellipse.
 */
class ellipse : public padding
{
public:
	struct specific_parameters {
		/**
		 * @brief Fill color of the ellipse.
		 */
		styled<ruis::color> fill_color;

		/**
		 * @brief Stroke width of the ellipse.
		 * Undefined value means no stroke.
		 */
		styled<length> stroke_width;

		/**
		 * @brief Stroke color of the ellipse.
		 */
		styled<ruis::color> stroke_color;
	};

	struct parameters {
		ruis::padding::parameters padding;
		specific_parameters specific;
	};

private:
	specific_parameters params;

public:
	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

	ellipse(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params,
		widget_list children
	);

	ellipse(const ellipse&) = delete;
	ellipse& operator=(const ellipse&) = delete;

	ellipse(ellipse&&) = delete;
	ellipse& operator=(ellipse&&) = delete;

	~ellipse() override = default;

	void render(const ruis::mat4& matrix) const override;

	void on_resize() override;

	void set_fill_color(styled<ruis::color> color);
	void set_stroke_color(styled<ruis::color> color);
	void set_stroke_width(styled<length> width);

	const color& get_stroke_color() const noexcept
	{
		return this->params.stroke_color.get();
	}

	const color& get_fill_color() const noexcept
	{
		return this->params.fill_color.get();
	}

private:
	bool has_stroke() const
	{
		return !this->params.stroke_width.get().is_undefined();
	}

	void update_vaos();

	ruis::paint::ellipse_vao fill_vao;
	ruis::paint::ellipse_vao stroke_vao;
};

namespace make {
/**
 * @brief Construct 'ellipse' widget.
 * Default layout is pile.
 * @param context - ruis context.
 * @param params - 'ellipse' widget parameters.
 * @param children - children of the constructed 'ellipse' widget.
 * @return newly constructed 'ellipse' widget.
 */
utki::shared_ref<ruis::ellipse> ellipse(
	const utki::shared_ref<ruis::context>& context, //
	ruis::ellipse::all_parameters params,
	widget_list children = {}
);
} // namespace make

} // namespace ruis
