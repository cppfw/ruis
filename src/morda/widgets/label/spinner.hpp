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

#include "image.hpp"

#include "../../res/image.hpp"

#include "../../updateable.hpp"

namespace morda{

/**
 * @brief Spinning image label.
 * @param active - whether the spinner is initially active or not, can be true/false.
 */
class spinner :
		public image,
		public updateable
{
	real angle = 0;
public:
	spinner(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	void set_active(bool active);

	void render(const matrix4& matrix)const override;
private:
	void update(uint32_t dt_ms)override;
};

}
