/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include <utki/shared_ref.hpp>

namespace ruis::render {

class context;

class texture_cube
{
	const utki::shared_ref<const ruis::render::context> rendering_context;

protected:
	texture_cube(utki::shared_ref<const ruis::render::context> rendering_context) :
		rendering_context(std::move(rendering_context))
	{}

public:
	texture_cube(const texture_cube&) = delete;
	texture_cube& operator=(const texture_cube&) = delete;

	texture_cube(texture_cube&&) = delete;
	texture_cube& operator=(texture_cube&&) = delete;

	virtual ~texture_cube() = default;
};

} // namespace ruis::render
