/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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
#include "../base/oriented_widget.hpp"

namespace morda {

/**
 * @brief Linear container widget.
 * Linear container lays out its child widgets in a row from left to right or in a column from top to bottom.
 */
class linear_container : public container, public oriented_widget
{
	linear_container(const linear_container&) = delete;
	linear_container& operator=(const linear_container&) = delete;

public:
	linear_container(const utki::shared_ref<morda::context>& c, const treeml::forest& desc, bool vertical);
};

} // namespace morda
