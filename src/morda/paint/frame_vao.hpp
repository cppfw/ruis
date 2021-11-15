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

#include "../render/renderer.hpp"

namespace morda{

class frame_vao{
	std::shared_ptr<morda::vertex_array> vao;
	
	std::shared_ptr<morda::renderer> renderer;
public:

	frame_vao(){}

	frame_vao(std::shared_ptr<morda::renderer> r, vector2 dims, vector2 thickness);

	void render(const matrix4& matrix, uint32_t color)const;
};

}
