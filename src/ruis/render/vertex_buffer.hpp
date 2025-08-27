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

#include <cstddef>

#include <utki/shared_ref.hpp>

namespace ruis::render {

class context;

class vertex_buffer
{
	const utki::shared_ref<const ruis::render::context> rendering_context;

public:
	const size_t size;

protected:
	vertex_buffer(
		utki::shared_ref<const ruis::render::context> rendering_context, //
		size_t size
	) :
		rendering_context(std::move(rendering_context)),
		size(size)
	{}

public:
	vertex_buffer(const vertex_buffer&) = delete;
	vertex_buffer& operator=(const vertex_buffer&) = delete;

	vertex_buffer(vertex_buffer&&) = delete;
	vertex_buffer& operator=(vertex_buffer&&) = delete;

	virtual ~vertex_buffer() = default;
};

} // namespace ruis::render
