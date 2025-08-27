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

std::vector<const ruis::render::context*> ruis::render::context::cur_context_stack;
std::vector<const ruis::render::context*> ruis::render::context::existing_contexts_list;

context::context(
	utki::shared_ref<ruis::render::native_window> native_window, //
	parameters params
) :
	native_window(std::move(native_window)),
	initial_matrix(std::move(params.initial_matrix))
{
	this->existing_contexts_list.push_back(this);

	// TODO: document this behaviour in doxygen that first created context becomes bound by default
	if (cur_context_stack.empty()) {
		// this created context is the only one existing context at the moment, bind it just to have some context always bound
		this->native_window.get().bind_rendering_context();
		cur_context_stack.push_back(this);
	}
}

context::~context()
{
	// remove this context from list of existing contexts
	auto i = std::find(existing_contexts_list.begin(), existing_contexts_list.end(), this);
	utki::assert(i != existing_contexts_list.end(), SL);
	existing_contexts_list.erase(i);

	if (this->is_current()) {
		cur_context_stack.pop_back();
		if (cur_context_stack.empty()) {
			if (!existing_contexts_list.empty()) {
				// there are no contexts in bound contexts stack, but
				// there are still some contexts existing, bind first one of them
				auto c = existing_contexts_list.front();
				c->native_window.get().bind_rendering_context();
				cur_context_stack.push_back(c);
			}
		} else {
			// bind the previous context
			cur_context_stack.back()->native_window.get().bind_rendering_context();
		}
	} else {
		auto i = std::find(
			cur_context_stack.begin(), //
			cur_context_stack.end(),
			this
		);
		if (i != cur_context_stack.end()) {
			cur_context_stack.erase(i);
		}
		// this context was not the current one, so the cur_context_stack should still be not empty
		utki::assert(!cur_context_stack.empty(), SL);
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

	utki::scope_exit restore_old_cc_scope_exit([&]() {
		utki::assert(this->is_current(), SL);
		cur_context_stack.pop_back();
		if (!cur_context_stack.empty()) {
			// bind the previous context
			cur_context_stack.back()->native_window.get().bind_rendering_context();
		}
	});

	cur_context_stack.push_back(this);
	this->native_window.get().bind_rendering_context();

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
