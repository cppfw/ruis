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

#include <utki/shared.hpp>

#include "shaders/coloring_shader.hpp"
#include "shaders/coloring_texturing_shader.hpp"
#include "shaders/shader.hpp"
#include "shaders/texturing_shader.hpp"

namespace ruis::render {
class context : public std::enable_shared_from_this<context>
{
protected:
	utki::shared_ref<context> get_shared_ref()
	{
		return utki::make_shared_from(*this);
	}

public:
	context() = default;

	context(const context&) = delete;
	context& operator=(const context&) = delete;

	context(context&&) = delete;
	context& operator=(context&&) = delete;

	virtual ~context() = default;

	struct shaders {
		std::unique_ptr<texturing_shader> pos_tex;
		std::unique_ptr<coloring_shader> color_pos;
		std::unique_ptr<coloring_shader> color_pos_lum;
		std::unique_ptr<shader> pos_clr;
		std::unique_ptr<coloring_texturing_shader> color_pos_tex;
		std::unique_ptr<coloring_texturing_shader> color_pos_tex_alpha;
	};

	virtual utki::shared_ref<shaders> create_shaders() = 0;
};
} // namespace ruis::render