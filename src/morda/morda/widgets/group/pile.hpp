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

#include "../container.hpp"

namespace morda{

/**
 * @brief Pile container widget.
 * Pile container is a container which lays out its children in a pile, each widget is centered inside of the container.
 * From GUI script it can be instantiated as 'pile'.
 */
class pile : public container{
public:
	pile(const pile&) = delete;
	pile& operator=(const pile&) = delete;

	/**
	 * @brief Constructor.
	 * @param c - context.
	 * @param desc - description of the widget.
	 */
	pile(std::shared_ptr<morda::context> c, const treeml::forest& desc);

public:
	morda::vector2 measure(const morda::vector2& quotum)const override;

	void lay_out() override;
};

}
