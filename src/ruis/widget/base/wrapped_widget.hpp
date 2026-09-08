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

#include <concepts>

#include "../widget.hpp"

namespace ruis {

struct container_tag{};
struct text_field_tag{};

/**
 * @brief Base class for widgets that wrap other widgets.
 *
 * The wrapped_widget class template provides common interface to get the wrapped widget.
 *
 * In the class hierarchy, the wrapped_widget must be initialized after the wrapped
 * widget's ancestor container to ensure proper parent-child relationships.
 *
 * @tparam widget_type - The type of the widget being wrapped. Must be a widget type
 *                      (i.e., derived from ruis::widget).
 */
template <typename widget_type>
requires std::derived_from<widget_type, widget>
class wrapped_widget : virtual public widget
{
	widget_type& bare;

protected:
	/**
	 * @brief Construct a wrapped_widget.
	 *
	 * @param context Shared reference to the ruis context.
	 * @param wrapped Reference to the widget being wrapped.
	 *                 This widget must have this wrapped_widget as an ancestor.
	 */
	wrapped_widget(
		utki::shared_ref<ruis::context> context, //
		widget_type& bare
	) :
		widget(std::move(context), {}, {}),
		bare(bare)
	{
		utki::assert(this->bare.has_ancestor(*this));
	}

public:
	/**
	 * @brief Get a mutable reference to the wrapped widget.
	 *
	 * @return Reference to the wrapped widget.
	 */
	widget_type& get_bare() noexcept
	{
		return this->bare;
	}

	/**
	 * @brief Get a const reference to the wrapped widget.
	 *
	 * @return Const reference to the wrapped widget.
	 */
	const widget_type& get_bare() const noexcept
	{
		return this->bare;
	}
};
} // namespace ruis