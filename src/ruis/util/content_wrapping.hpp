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

#include "../widget/container.hpp"

namespace ruis {

/**
 * @brief Content wrapping widget base.
 * The base class for widgets which are essentially containers,
 * but enchancing functionality of simple container widget,
 * for example, by adding padding to the contents or adding
 * some special control widgets on top of the content.
 */
class content_wrapping
{
protected:
	const utki::shared_ref<ruis::container> content_container;

	content_wrapping(utki::shared_ref<container> content_container);

public:
	/**
	 * @brief Get content container.
	 * @return The content container. This is where the child widgets are stored.
	 */
	container& content()
	{
		return this->content_container.get();
	}

	/**
	 * @brief Get content container.
	 * @return The content container. This is where the child widgets are stored.
	 */
	const container& content() const
	{
		return this->content_container.get();
	}
};

} // namespace ruis
