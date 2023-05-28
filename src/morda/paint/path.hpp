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

#include <vector>

#include "../config.hpp"

namespace morda {

class path
{
	std::vector<morda::vector2> points = {{morda::vector2(0)}};

public:
	path() = default;

	path(const path&) = delete;
	path& operator=(const path&) = delete;

	void line_to(morda::vector2 abs_pos);

	void line_to(morda::real x, morda::real y)
	{
		this->line_to(morda::vector2(x, y));
	}

	void line_by(morda::vector2 rel_pos);

	void cubic_to(morda::vector2 abs_p1, morda::vector2 abs_p2, morda::vector2 abs_p3);
	void cubic_by(morda::vector2 rel_p1, morda::vector2 rel_p2, morda::vector2 rel_p3);

	struct vertices {
		std::vector<morda::vector2> pos;
		std::vector<morda::real> alpha;

		std::vector<uint16_t> in_indices;
		std::vector<uint16_t> out_indices;
	};

	vertices stroke(
		morda::real half_width = morda::real(0.5f),
		morda::real antialias_width = morda::real(1.0f),
		morda::real antialias_alpha = morda::real(0.35f)
	) const;
};

} // namespace morda
