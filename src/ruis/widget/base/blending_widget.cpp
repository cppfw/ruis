/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include "blending_widget.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace ruis;

blending_widget::blending_widget(
	utki::shared_ref<ruis::context> context, //
	parameters params
) :
	widget(std::move(context), {}, {}),
	params(std::move(params))
{}

void blending_widget::set_blending_to_renderer() const
{
	const auto& r = this->ctx().ren();
	r.ctx().enable_blend(this->is_blending_enabled());
	if (this->is_blending_enabled()) {
		r.ctx().set_blend_func(
			this->params.factors.src,
			this->params.factors.dst,
			this->params.factors.src_alpha,
			this->params.factors.dst_alpha
		);
	}
}

void blending_widget::set_blending_enabled(bool enable)
{
	if (this->params.enabled == enable) {
		return;
	}
	this->params.enabled = enable;
	this->on_blending_change();
}

void blending_widget::set_blending_factors(const blending_factors& params)
{
	if (this->params.factors == params) {
		return;
	}
	this->params.factors = params;
	this->on_blending_change();
}
