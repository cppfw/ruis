/*
morda GUI framework

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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

#include "linear_container.hpp"

namespace morda{

/**
 * @brief Vertical container widget.
 * column is a vertical variant of linear container. From GUI scripts it can be instantiated as "column".
 */
class column : public linear_container{
public:
	column(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			widget(std::move(c), desc),
			linear_container(nullptr, desc, true)
	{}

	column(const column&) = delete;
	column& operator=(const column&) = delete;
};

}
