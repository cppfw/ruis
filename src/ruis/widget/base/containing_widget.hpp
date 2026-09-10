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

#include "../container.hpp"

namespace ruis {

/**
 * @brief Base class for widgets that wrap a container widget.
 *
 * The containing_widget class provides a common interface to get the
 * wrapped container that holds the widget's contents.
 *
 * In the class hierarchy, the containing_widget must be initialized after the
 * contents' ancestor container to ensure proper parent-child relationships.
 */
class containing_widget : virtual public widget
{
	container& contents;

protected:
	/**
	 * @brief Construct a containing_widget.
	 *
	 * @param context Shared reference to the ruis context.
	 * @param contents Reference to the container holding the widget's contents.
	 *                  This container must have this containing_widget as an ancestor.
	 */
	containing_widget(
		const utki::shared_ref<ruis::context>& context, //
		container& contents
	) :
		widget(context, {}, {}),
		contents(contents)
	{
		utki::assert(this->contents.has_ancestor(*this));
	}

public:
	/**
	 * @brief Get a mutable reference to the contents container.
	 *
	 * @return Reference to the contents container.
	 */
	container& get_container() noexcept
	{
		return this->contents;
	}

	/**
	 * @brief Get a const reference to the contents container.
	 *
	 * @return Const reference to the contents container.
	 */
	const container& get_container() const noexcept
	{
		return this->contents;
	}
};
} // namespace ruis
