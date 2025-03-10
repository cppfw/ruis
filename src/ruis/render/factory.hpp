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

#include <vector>

#include <r4/vector.hpp>
#include <rasterimage/image_variant.hpp>
#include <utki/shared_ref.hpp>
#include <utki/span.hpp>

#include "context.hpp"
#include "frame_buffer.hpp"
#include "index_buffer.hpp"
#include "texture_2d.hpp"
#include "texture_cube.hpp"
#include "texture_depth.hpp"
#include "texture_stencil.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"

namespace ruis::render {

class renderer;

class factory
{
	friend class renderer;

	// this variable is initialized by the renderer which owns the factory
	std::weak_ptr<renderer> renderer_v;

protected:
	utki::shared_ref<renderer> get_renderer()
	{
		auto r = this->renderer_v.lock();
		ASSERT(r)
		return utki::shared_ref<renderer>(std::move(r));
	}

	factory(utki::shared_ref<ruis::render::context> render_context) :
		render_context(std::move(render_context))
	{}

public:
	const utki::shared_ref<ruis::render::context> render_context;

	factory(const factory&) = delete;
	factory& operator=(const factory&) = delete;

	factory(factory&&) = delete;
	factory& operator=(factory&&) = delete;

	virtual ~factory() = default;
};

} // namespace ruis::render
