/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include <rasterimage/dimensioned.hpp>

#include "texture_2d.hpp"

namespace ruis::render {

class frame_buffer : public rasterimage::dimensioned
{
protected:
	frame_buffer(utki::shared_ref<texture_2d> color) :
		rasterimage::dimensioned(color.get().dims()),
		color(std::move(color))
	{}

public:
	const utki::shared_ref<texture_2d> color;

	frame_buffer(const frame_buffer&) = delete;
	frame_buffer& operator=(const frame_buffer&) = delete;

	frame_buffer(frame_buffer&&) = delete;
	frame_buffer& operator=(frame_buffer&&) = delete;

	virtual ~frame_buffer() = default;

private:
};

} // namespace ruis::render
