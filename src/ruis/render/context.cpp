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

const ruis::render::context* ruis::render::context::cur_context = nullptr;

context::context(
	utki::shared_ref<ruis::render::native_window> native_window, //
	parameters params
) :
	native_window(std::move(native_window)),
	initial_matrix(std::move(params.initial_matrix))
{
	if (!cur_context) {
		// this created context is the only one existing context at the moment, bind it just to have some context always bound
		this->native_window.get().bind_rendering_context();
		cur_context = this;
	}
}

context::~context()
{
	// if the context is bound during destruction then it is the last context
	if (this->is_current()) {
		cur_context = nullptr;
	}
}

void context::apply(std::function<void()> func)
{
	utki::assert(func, SL);

	if (this->is_current()) {
		// this context is already current
		func();
		return;
	}

	auto old_cc = cur_context;
	utki::scope_exit restore_old_cc_scope_exit([&]() {
		if (!old_cc) {
			// There was no current context, leave current one bound.
			return;
		}
		old_cc->native_window.get().bind_rendering_context();
		cur_context = old_cc;
	});

	this->native_window.get().bind_rendering_context();
	cur_context = this;

	utki::assert(this->is_current(), SL);

	func();
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
