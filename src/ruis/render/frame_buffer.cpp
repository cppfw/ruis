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

#include "frame_buffer.hpp"

#include <utki/string.hpp>

using namespace ruis::render;

frame_buffer::frame_buffer( //
	utki::shared_ref<ruis::render::renderer> renderer,
	std::shared_ptr<texture_2d> color,
	std::shared_ptr<texture_depth> depth,
	std::shared_ptr<texture_stencil> stencil
) :
	rasterimage::dimensioned([&]() {
		rasterimage::dimensioned* cur_tex = nullptr;

		std::array<rasterimage::dimensioned*, 2> tex_attachments = {color.get(), depth.get()};

		for (auto& t : tex_attachments) {
			if (!t) {
				continue;
			}
			if (cur_tex && t->dims() != cur_tex->dims()) {
				throw std::logic_error(utki::cat(
					"frame_buffer(): texture attachments have different dimensions: (",
					cur_tex->dims(),
					") and (",
					t->dims(),
					")"
				));
			}
			cur_tex = t;
		}

		if (!cur_tex) {
			throw std::logic_error("frame_buffer(): no any texture attachments given");
		}

		return cur_tex->dims();
	}()),
	renderer(std::move(renderer)),
	color(std::move(color)),
	depth(std::move(depth)),
	stencil(std::move(stencil))
{}
