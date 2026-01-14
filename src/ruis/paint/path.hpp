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

#include <vector>

#include "../config.hpp"

namespace ruis::paint {

// TODO: doxygen
class path
{
	std::vector<ruis::vector2> points = {{ruis::vector2(0)}};

public:
	path() = default;

	path(const path&) = delete;
	path& operator=(const path&) = delete;

	path(path&&) = delete;
	path& operator=(path&&) = delete;

	~path() = default;

	void line_to(ruis::vector2 abs_pos);

	void line_to(
		ruis::real x, //
		ruis::real y
	)
	{
		this->line_to(ruis::vector2(x, y));
	}

	void line_by(ruis::vector2 rel_pos);

	void cubic_to(
		ruis::vector2 abs_p1, //
		ruis::vector2 abs_p2, //
		ruis::vector2 abs_p3
	);
	void cubic_by(
		ruis::vector2 rel_p1, //
		ruis::vector2 rel_p2, //
		ruis::vector2 rel_p3
	);

	struct vertices {
		std::vector<ruis::vector2> pos;
		std::vector<ruis::real> alpha;

		std::vector<uint16_t> in_indices;
		std::vector<uint16_t> out_indices;
	};

	constexpr static auto default_half_width = 0.5;
	constexpr static auto default_antialias_width = 1;
	constexpr static auto default_antialias_alpha = 0.35;

	vertices stroke(
		ruis::real half_width = ruis::real(default_half_width),
		ruis::real antialias_width = ruis::real(default_antialias_width),
		ruis::real antialias_alpha = ruis::real(default_antialias_alpha)
	) const;
};

} // namespace ruis::paint
