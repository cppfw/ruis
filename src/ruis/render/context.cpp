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

#include "context.hpp"

using namespace ruis::render;

context::context(parameters params) :
	initial_matrix(std::move(params.initial_matrix))
{}

void context::apply(std::function<void()> func)
{
	if (this->is_current()) {
		func();
		return;
	}

	throw std::logic_error("context::apply(): the context is non-current, switching context is not yet implemented");
}

void context::set_framebuffer(frame_buffer* fb)
{
	this->set_framebuffer_internal(fb);
	if (fb) {
		this->cur_fb = fb->weak_from_this();
	} else {
		this->cur_fb.reset();
	}
}

void context::set_simple_alpha_blending()
{
	this->enable_blend(true);
	this->set_blend_func(
		context::blend_factor::src_alpha,
		context::blend_factor::one_minus_src_alpha,
		context::blend_factor::one,
		context::blend_factor::one_minus_src_alpha
	);
}
