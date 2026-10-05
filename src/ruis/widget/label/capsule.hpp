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

#include "../../paint/capsule_vao.hpp"

#include "padding.hpp"

namespace ruis {

/**
 * @brief Capsule widget.
 * A capsule (a rectangle with fully rounded ends) of a single fill color with
 * an optional stroke. The capsule is horizontal: its diameter is the widget's
 * height and its length is the widget's width.
 */
class capsule : public padding
{
public:
	struct specific_parameters {
		/**
		 * @brief Fill color of the capsule.
		 */
		styled<ruis::color> fill_color;

		/**
		 * @brief Stroke width of the capsule.
		 * Undefined value means no stroke.
		 */
		styled<length> stroke_width;

		/**
		 * @brief Stroke color of the capsule.
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

	capsule(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params,
		widget_list children
	);

	capsule(const capsule&) = delete;
	capsule& operator=(const capsule&) = delete;

	capsule(capsule&&) = delete;
	capsule& operator=(capsule&&) = delete;

	~capsule() override = default;

	void render(const ruis::mat4& matrix) const override;

	void on_resize() override;

	void set_fill_color(styled<ruis::color> color);
	void set_stroke_color(styled<ruis::color> color);
	void set_stroke_width(styled<length> width);

	const ruis::color& get_fill_color() const noexcept
	{
		return this->params.fill_color.get();
	}

	const ruis::color& get_stroke_color() const noexcept
	{
		return this->params.stroke_color.get();
	}

private:
	bool has_stroke() const noexcept
	{
		return !this->params.stroke_width.get().is_undefined();
	}

	void update_vaos();

	ruis::paint::capsule_vao stroke_vao;
	ruis::paint::capsule_vao fill_vao;
};

namespace make {
/**
 * @brief Construct a 'capsule' widget.
 * @param context - ruis context.
 * @param params - 'capsule' widget parameters.
 * @param children - children of the constructed 'capsule' widget.
 * @return newly constructed 'capsule' widget.
 */
utki::shared_ref<ruis::capsule> capsule(
	const utki::shared_ref<ruis::context>& context, //
	ruis::capsule::all_parameters params,
	widget_list children = {}
);
} // namespace make

} // namespace ruis
