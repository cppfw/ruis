/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

namespace {
const std::map<std::string, render::renderer::blend_factor> blend_factor_names = {
	{					"zero",					 render::renderer::blend_factor::zero},
	{					 "one",					  render::renderer::blend_factor::one},
	{			   "src_color",                render::renderer::blend_factor::src_color},
	{	 "one_minus_src_color",      render::renderer::blend_factor::one_minus_src_color},
	{			   "dst_color",                render::renderer::blend_factor::dst_color},
	{	 "one_minus_dst_color",      render::renderer::blend_factor::one_minus_dst_color},
	{			   "src_alpha",                render::renderer::blend_factor::src_alpha},
	{	 "one_minus_src_alpha",      render::renderer::blend_factor::one_minus_src_alpha},
	{			   "dst_alpha",                render::renderer::blend_factor::dst_alpha},
	{	 "one_minus_dst_alpha",      render::renderer::blend_factor::one_minus_dst_alpha},
	{		  "constant_color",           render::renderer::blend_factor::constant_color},
	{"one_minus_constant_color", render::renderer::blend_factor::one_minus_constant_color},
	{		  "constant_alpha",           render::renderer::blend_factor::constant_alpha},
	{"one_minus_constant_alpha", render::renderer::blend_factor::one_minus_constant_alpha},
	{	  "src_alpha_saturate",       render::renderer::blend_factor::src_alpha_saturate}
};

render::renderer::blend_factor blend_factor_from_string(const std::string& s)
{
	auto i = blend_factor_names.find(s);
	if (i == blend_factor_names.end()) {
		return render::renderer::blend_factor::one;
	}
	return i->second;
}
} // namespace

blending_widget::blending_widget(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc)
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "blend") {
			this->params.enabled = get_property_value(p).to_bool();
		} else if (p.value == "blend_src") {
			this->params.factors.src = blend_factor_from_string(get_property_value(p).string);
		} else if (p.value == "blend_dst") {
			this->params.factors.dst = blend_factor_from_string(get_property_value(p).string);
		} else if (p.value == "blend_src_alpha") {
			this->params.factors.src_alpha = blend_factor_from_string(get_property_value(p).string);
		} else if (p.value == "blend_dst_alpha") {
			this->params.factors.dst_alpha = blend_factor_from_string(get_property_value(p).string);
		}
	}
}

blending_widget::blending_widget(utki::shared_ref<ruis::context> context, parameters params) :
	widget(std::move(context), widget::all_parameters{}),
	params(std::move(params))
{}

void blending_widget::set_blending_to_renderer() const
{
	auto& r = this->context.get().renderer.get();
	r.set_blend_enabled(this->is_blending_enabled());
	if (this->is_blending_enabled()) {
		r.set_blend_func(
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
