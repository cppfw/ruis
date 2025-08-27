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

class index_buffer
{
	const utki::shared_ref<const ruis::render::context> rendering_context;

protected:
	index_buffer(utki::shared_ref<const ruis::render::context> rendering_context) :
		rendering_context(std::move(rendering_context))
	{}

public:
	index_buffer(const index_buffer&) = delete;
	index_buffer& operator=(const index_buffer&) = delete;

	index_buffer(index_buffer&&) = delete;
	index_buffer& operator=(index_buffer&&) = delete;

	virtual ~index_buffer() = default;
};

} // namespace ruis::render
