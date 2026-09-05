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

/**
 * @brief Base class for widgets that decorate other widgets.
 *
 * The decorated_widget class template provides common interface to get the decorated widget.
 *
 * In the class hierarchy, the decorated_widget must be initialized after the decorated
 * widget's ancestor container to ensure proper parent-child relationships.
 *
 * @tparam widget_type The type of the widget being decorated. Must be a widget type
 *                     (i.e., derived from ruis::widget).
 */
template <typename widget_type>
	requires std::derived_from<widget_type, widget>
class decorated_widget : virtual public widget {
    widget_type& decorated;

protected:
    /**
     * @brief Construct a decorated_widget.
     *
     * @param context Shared reference to the ruis context.
     * @param decorated Reference to the widget being decorated.
     *                 This widget must have this decorated_widget as an ancestor.
     */
    decorated_widget(
        utki::shared_ref<ruis::context> context, //
        widget_type& decorated
    ) :
        widget(std::move(context), {}, {}),
        decorated(decorated)
    {
        utki::assert(this->decorated.has_ancestor(*this));
    }

public:
    /**
     * @brief Get a mutable reference to the decorated widget.
     *
     * @return Reference to the decorated widget.
     */
    widget_type& get_decorated() noexcept
    {
        return this->decorated;
    }

    /**
     * @brief Get a const reference to the decorated widget.
     *
     * @return Const reference to the decorated widget.
     */
    const widget_type& get_decorated() const noexcept
    {
        return this->decorated;
    }
};
}