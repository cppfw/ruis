/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

#include "../widget.hpp"

namespace morda{

/**
 * @brief Widget for intercepting resize events.
 * From GUI script it can be instantiated as "resize_proxy".
 */
class resize_proxy : virtual public widget{
public:
	resize_proxy(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			widget(std::move(c), desc)
	{}

	resize_proxy(const resize_proxy&) = delete;
	resize_proxy& operator=(const resize_proxy&) = delete;

	void on_resize()override;

	/**
	 * @brief Resize event signal.
	 * Emitted when this widget is resized.
	 */
	std::function<void(resize_proxy& w)> resize_handler;
};

}
