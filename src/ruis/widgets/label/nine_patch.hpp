/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "../../container.hpp"
#include "../../res/nine_patch.hpp"
#include "../base/blending_widget.hpp"

#include "image.hpp"

namespace ruis {

/**
 * @brief Nine patch widget.
 * Nine patch widget displays a nine-patch and can hold child widgets in its central area.
 * From GUI script it can be instantiated as "nine_patch".
 *
 * @param left - width of left border, in length units.
 * @param right - width of right border, in length units.
 * @param top - height of top border, in length units.
 * @param bottom - height of bottom border, in length units.
 * @param center_visible - whether the central part of nine-patch image is visible (true) or not (false).
 * @param image - reference to a nine-patch resource.
 */
class nine_patch :
	public virtual widget, //
	public blending_widget,
	private container
{
	std::shared_ptr<res::nine_patch::image_matrix> img_res_matrix;

	const std::array<std::array<std::reference_wrapper<image>, 3>, 3> img_widgets_matrix;

	container& inner_content;

protected:
	bool on_mouse_move(const mouse_move_event& e) override
	{
		return this->container::on_mouse_move(e);
	}

	bool on_mouse_button(const mouse_button_event& e) override
	{
		return this->container::on_mouse_button(e);
	}

public:
	struct parameters {
		std::shared_ptr<const res::nine_patch> nine_patch;
		std::shared_ptr<const res::nine_patch> disabled_nine_patch;
		sides<real> borders = sides<real>(lp::min);
	};

private:
	parameters params;

public:
	nine_patch(
		utki::shared_ref<ruis::context> context,
		widget::parameters widget_params,
		blending_widget::parameters blending_widget_params,
		parameters params,
		utki::span<const utki::shared_ref<widget>> children
	);

	nine_patch(const nine_patch&) = delete;
	nine_patch& operator=(const nine_patch&) = delete;

	nine_patch(nine_patch&&) = delete;
	nine_patch& operator=(nine_patch&&) = delete;

	~nine_patch() override = default;

	nine_patch(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc);

	void set_nine_patch(std::shared_ptr<const res::nine_patch> np);

	void set_disabled_nine_patch(std::shared_ptr<const res::nine_patch> np);

	/**
	 * @brief Get content container.
	 * @return The content container. This is where the child widgets are stored.
	 */
	container& content()
	{
		return this->inner_content;
	}

	void render(const ruis::matrix4& matrix) const override;

	/**
	 * @brief Show/hide central part of nine-patch.
	 * @param visible - show (true) or hide (false) central part of the nine-patch.
	 */
	void set_center_visible(bool visible);

	/**
	 * @brief Set border settings.
	 * Border values are in pixels or min_c.
	 * @param borders - border values to set.
	 */
	void set_borders(sides<real> borders)
	{
		this->params.borders = borders;
		this->apply_images();
		this->invalidate_layout();
	}

	/**
	 * @brief Get current border settings.
	 * Border values are in pixels or min_c.
	 * @return Current borders.
	 */
	decltype(params.borders) get_borders() const noexcept
	{
		return this->params.borders;
	}

	sides<real> get_actual_borders() const noexcept;

	void on_blending_change() override;

	void on_enabled_change() override;

private:
	void apply_images();
	void update_images();
};

} // namespace ruis
