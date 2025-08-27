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

#include <rasterimage/dimensioned.hpp>
#include <utki/shared_ref.hpp>

#include "texture_2d.hpp"
#include "texture_depth.hpp"
#include "texture_stencil.hpp"

namespace ruis::render {

class context;

class frame_buffer :
	public rasterimage::dimensioned, //
	public std::enable_shared_from_this<frame_buffer>
{
	// in OpneGL framebuffer  objects are not shred between contexts,
	// so the reference stored here is for non-const context object which owns the framebuffer
	const utki::shared_ref<ruis::render::context> rendering_context;

protected:
	frame_buffer( //
		utki::shared_ref<ruis::render::context> rendering_context,
		std::shared_ptr<texture_2d> color,
		std::shared_ptr<texture_depth> depth,
		std::shared_ptr<texture_stencil> stencil
	);

public:
	const std::shared_ptr<texture_2d> color;
	const std::shared_ptr<texture_depth> depth;
	const std::shared_ptr<texture_stencil> stencil;

	frame_buffer(const frame_buffer&) = delete;
	frame_buffer& operator=(const frame_buffer&) = delete;

	frame_buffer(frame_buffer&&) = delete;
	frame_buffer& operator=(frame_buffer&&) = delete;

	virtual ~frame_buffer() = default;

	void apply(std::function<void()> func);

private:
};

} // namespace ruis::render
