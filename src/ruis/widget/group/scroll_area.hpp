/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "../container.hpp"

namespace ruis {

/**
 * @brief Scroll area container widget.
 * Scroll area is a container which can add an offset to its children widget positions.
 * From GUI scripts it can be instantiated as "scroll_area".
 * Note, that Scrollarea has same layout parameters as simple container and those work similarly,
 * except 'max' value. If layout dimension is specified as 'max' then child widget will be stretched to the
 * parent (scroll_area) size in case child's minimal size is less than scroll_area size, otherwise child will be
 * assigned its minimal size.
 */
class scroll_area : public container
{
	// offset from top left corner
	vector2 cur_scroll_pos = vector2(0);

	// cached dimensions of the invisible contnets part, which goes beyond right and bottom edge of the scroll_area
	vector2 invisible_dims;

	// cached scroll factor
	vector2 cur_scroll_factor;

protected:
	vector2 dims_for_widget(const widget& w) const;

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
	};

	scroll_area(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		utki::span<const utki::shared_ref<widget>> children
	);

	scroll_area(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	scroll_area(const scroll_area&) = delete;
	scroll_area& operator=(const scroll_area&) = delete;

	scroll_area(scroll_area&&) = delete;
	scroll_area& operator=(scroll_area&&) = delete;

	~scroll_area() override = default;

	bool on_mouse_button(const mouse_button_event& event) override;

	bool on_mouse_move(const mouse_move_event& event) override;

	void render(const ruis::matrix4& matrix) const override;

	ruis::vector2 measure(const ruis::vector2& quotum) const override
	{
		// NOLINTNEXTLINE(bugprone-parent-virtual-call, "we want to cancel container::measure() override")
		return this->widget::measure(quotum);
	}

	void on_lay_out() override;

	void on_children_change() override;

	/**
	 * @brief Get current scroll position.
	 * @return Current scrolling position in pixels.
	 */
	const vector2& get_scroll_pos() const
	{
		return this->cur_scroll_pos;
	}

	/**
	 * @brief Set scroll position.
	 * The scroll position will be clamped to effective dimensions of scroll_area's contents.
	 * @param new_scroll_pos - new scroll position.
	 */
	void set_scroll_pos(const vector2& new_scroll_pos);

	/**
	 * @brief Set scroll position as factor.
	 * @param factor - factor with components from range [0:1].
	 */
	void set_scroll_factor(const vector2& factor);

	/**
	 * @brief Get current scroll position as factor.
	 * @return Current scroll position as factor with components from range [0:1].
	 */
	const vector2& get_scroll_factor() const
	{
		return this->cur_scroll_factor;
	}

	vector2 get_visible_area_fraction() const noexcept;

	virtual void on_scroll_pos_change();

	std::function<void(scroll_area&)> scroll_change_handler;

private:
	void update_invisible_dims();

	void update_scroll_factor();

	void clamp_scroll_pos();

	void arrange_widgets();
};

namespace make {
inline utki::shared_ref<ruis::scroll_area> scroll_area(
	utki::shared_ref<ruis::context> context, //
	scroll_area::all_parameters params,
	utki::span<const utki::shared_ref<ruis::widget>> children = {}
)
{
	return utki::make_shared<ruis::scroll_area>(
		std::move(context), //
		std::move(params),
		children
	);
}

} // namespace make

} // namespace ruis
