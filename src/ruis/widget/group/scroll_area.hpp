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

#include "../base/containing_widget.hpp"
#include "../container.hpp"

namespace ruis {

/**
 * @brief Scroll area widget.
 * A scroll area is a widget which aggregates a content container widget holding the actual contents
 * and provides scrolling of the contents. The content container is created in the constructor and
 * holds the child widgets passed to the constructor. Mouse events are forwarded to the content
 * container, keyboard events propagate to it via the usual widget hierarchy.
 * From GUI scripts it can be instantiated as "scroll_area".
 * The child widgets keep their positions and have the same layout parameters as in a simple container:
 * 'fill' and 'max' children are stretched to the content container size, 'min' children are assigned
 * their minimal size and 'length' children are assigned the fixed length.
 * The content container is resized in on_lay_out() to the maximum of the scroll area size and the
 * minimal size needed to fit all the child widgets; the part of the contents which goes beyond the
 * right and bottom edge of the scroll area is scrollable.
 */
// TODO: is this lint suppression needed?
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via widget")

class scroll_area :
	virtual public widget, //
	// The private container base exists to host the content container as a child:
	// the widget tree parent/child mechanics live in container (widget::parent_container
	// is of type container*), so a widget can only have children by being a container.
	// The inheritance is private to hide the container interface from user code;
	// the content is accessed via containing_widget::get_container().
	private container, //
	public containing_widget
{
	// offset from top left corner
	vec2 cur_scroll_pos = vec2(0);

	// cached dimensions of the invisible contents part, which goes beyond right and bottom edge of the scroll_area
	vec2 invisible_dims;

	// cached scroll factor
	vec2 cur_scroll_factor;

private:
	// the content container holding the scroll area's contents
	utki::shared_ref<container> content_container;

public:
	struct parameters{
		ruis::container::parameters container;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

private:
	scroll_area(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters& params,
		utki::shared_ref<ruis::container> content_container
	);

public:
	scroll_area(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params,
		widget_list children
	);

	scroll_area(const scroll_area&) = delete;
	scroll_area& operator=(const scroll_area&) = delete;

	scroll_area(scroll_area&&) = delete;
	scroll_area& operator=(scroll_area&&) = delete;

	~scroll_area() override = default;

	event_status on_mouse_button(const mouse_button_event& event) override;

	event_status on_mouse_move(const mouse_move_event& event) override;

	void render(const ruis::mat4& matrix) const override;

	ruis::vec2 measure(
		const ruis::vec2& quotum, //
		const r4::vector2<measure_mode>& mode //
	) const override
	{
		// NOLINTNEXTLINE(bugprone-parent-virtual-call, "we want to cancel container::measure() override")
		return this->widget::measure(quotum, mode);
	}

	void on_lay_out() override;

	/**
	 * @brief Get current scroll position.
	 * @return Current scrolling position in pixels.
	 */
	const vec2& get_scroll_pos() const
	{
		return this->cur_scroll_pos;
	}

	/**
	 * @brief Set scroll position.
	 * The scroll position will be clamped to effective dimensions of scroll_area's contents.
	 * @param new_scroll_pos - new scroll position.
	 */
	void set_scroll_pos(const vec2& new_scroll_pos);

	/**
	 * @brief Scroll by delta.
	 * @param delta - amount to scroll by in pixels.
	 * @return Number of pixels actually scrolled.
	 */
	vec2 scroll_by(const vec2& delta);

	/**
	 * @brief Set scroll position as factor.
	 * @param factor - factor with components from range [0:1].
	 */
	void set_scroll_factor(const vec2& factor);

	/**
	 * @brief Get current scroll position as factor.
	 * @return Current scroll position as factor with components from range [0:1].
	 */
	const vec2& get_scroll_factor() const
	{
		return this->cur_scroll_factor;
	}

	vec2 get_visible_area_fraction() const noexcept;

	virtual void on_scroll_pos_change();

	std::function<void(scroll_area&)> scroll_change_handler;

private:
	void update_invisible_dims();

	void update_scroll_factor();

	void clamp_scroll_pos();
};

namespace make {
utki::shared_ref<ruis::scroll_area> scroll_area(
	const utki::shared_ref<ruis::context>& context, //
	ruis::scroll_area::all_parameters params,
	ruis::widget_list children = {}
);
} // namespace make

} // namespace ruis
