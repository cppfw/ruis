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

#include "path.hpp"

#include <cmath>

using namespace morda;

void path::line_to(morda::vector2 abs_pos)
{
	this->points.emplace_back(abs_pos);
}

void path::line_by(morda::vector2 rel_pos)
{
	ASSERT(this->points.size() != 0)
	this->line_to(this->points.back() + rel_pos);
}

void path::cubic_by(morda::vector2 rel_p1, morda::vector2 rel_p2, morda::vector2 rel_p3)
{
	ASSERT(this->points.size() != 0)
	auto& d = this->points.back();
	this->cubic_to(d + rel_p1, d + rel_p2, d + rel_p3);
}

void path::cubic_to(morda::vector2 p1, morda::vector2 p2, morda::vector2 p3)
{
	auto p0 = this->points.back();

	auto bezier = [p0, p1, p2, p3](morda::real t) {
		using utki::pow3;
		using utki::pow2;
		return pow3(1 - t) * p0 + 3 * t * pow2(1 - t) * p1 + 3 * pow2(t) * (1 - t) * p2 + pow3(t) * p3;
	};

	auto length_est = (p1 - p0).norm() + (p2 - p1).norm() + (p3 - p2).norm();

	const morda::real max_step_pixels = 10.0f; // 10 pixels

	auto num_steps = length_est / max_step_pixels;

	auto dt = 1 / num_steps;

	// NOTE: start from dt because 0th point is already there in the path
	for (morda::real t = dt; t < 1; t += dt) { // NOLINT(clang-analyzer-security.FloatLoopCounter)
		this->line_to(bezier(t));
	}
	this->line_to(bezier(1));
}

path::vertices path::stroke(morda::real half_width, morda::real antialias_width, morda::real antialias_alpha) const
{
	vertices ret;

	if (this->points.size() <= 1) {
		return ret;
	}

	const decltype(this->points)::value_type *prev = nullptr, *cur = nullptr, *next = nullptr;

	uint16_t in_index = 0;

	for (auto i = this->points.begin(); i != this->points.end(); ++i) {
		prev = cur;
		cur = &(*i);
		{
			auto ii = i;
			++ii;
			if (ii == this->points.end()) {
				next = nullptr;
			} else {
				next = &(*ii);
			}
		}

		morda::vector2 prev_normal = 0, next_normal;

		ASSERT(prev || next)

		if (prev) {
			using std::swap;
			prev_normal = (*cur - *prev).normalize();
			swap(prev_normal.x(), prev_normal.y());
			prev_normal.x() = -prev_normal.x();
		}
		if (next) {
			using std::swap;
			next_normal = (*next - *cur).normalize();
			swap(next_normal.x(), next_normal.y());
			next_normal.x() = -next_normal.x();
		} else {
			ASSERT(prev)
			next_normal = prev_normal;
		}
		if (!prev) {
			ASSERT(next)
			prev_normal = next_normal;
		}

		auto normal = (prev_normal + next_normal).normalize();

		auto miter_coeff = 1 / (normal * prev_normal);
		auto miter = miter_coeff * half_width;
		auto antialias_miter = miter_coeff * (half_width + antialias_width);

		using std::sqrt;
		using utki::pi;

		if (!prev) {
			ASSERT(next)
			ret.pos.push_back(
				(*cur) - normal * miter - normal.rot(-pi<morda::real>() / 4) * antialias_width * morda::real(sqrt(2))
			);
		} else if (!next) {
			ASSERT(prev)
			ret.pos.push_back(
				(*cur) - normal * miter - normal.rot(pi<morda::real>() / 4) * antialias_width * morda::real(sqrt(2))
			);
		} else {
			ret.pos.push_back((*cur) - normal * antialias_miter);
		}
		ret.alpha.push_back(0);
		++in_index;

		ret.pos.push_back((*cur) - normal * miter);
		ret.alpha.push_back(antialias_alpha);
		ret.in_indices.push_back(in_index);
		++in_index;

		ret.pos.push_back((*cur) + normal * miter);
		ret.alpha.push_back(antialias_alpha);
		ret.in_indices.push_back(in_index);
		++in_index;

		if (!prev) {
			ret.pos.push_back(
				(*cur) + normal * miter + normal.rot(pi<morda::real>() / 4) * antialias_width * morda::real(sqrt(2))
			);
		} else if (!next) {
			ret.pos.push_back(
				(*cur) + normal * miter + normal.rot(-pi<morda::real>() / 4) * antialias_width * morda::real(sqrt(2))
			);
		} else {
			ret.pos.push_back((*cur) + normal * antialias_miter);
		}
		ret.alpha.push_back(0);
		++in_index;
	}

	ret.out_indices.push_back(3);
	ret.out_indices.push_back(2);

	for (unsigned i = 0; i != this->points.size(); ++i) {
		ret.out_indices.push_back(4 * i);
		ret.out_indices.push_back(4 * i + 1);
	}

	for (auto i = unsigned(this->points.size() - 1); i != 0; --i) {
		ret.out_indices.push_back(4 * i + 3);
		ret.out_indices.push_back(4 * i + 2);
	}

	ret.out_indices.push_back(3);
	ret.out_indices.push_back(2);

	return ret;
}
