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

#include <tuple>
#include <utility>

namespace ruis {

class oriented
{
public:
	struct parameters {
		bool vertical = true;
	};

private:
	parameters params;

public:
	unsigned get_long_index() const noexcept
	{
		return this->params.vertical ? 1 : 0;
	}

	unsigned get_trans_index() const noexcept
	{
		return this->params.vertical ? 0 : 1;
	}

	std::tuple<unsigned, unsigned> get_long_trans_indices() const noexcept
	{
		if (this->params.vertical) {
			return {1, 0};
		} else {
			return {0, 1};
		}
	}

	oriented(parameters params) :
		params(std::move(params))
	{}

	bool is_vertical() const noexcept
	{
		return this->params.vertical;
	}
};

} // namespace ruis
