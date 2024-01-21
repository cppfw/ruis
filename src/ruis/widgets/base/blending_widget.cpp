/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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
const std::map<std::string, renderer::blend_factor> blend_factor_names = {
	{					"zero",					 renderer::blend_factor::zero},
	{					 "one",					  renderer::blend_factor::one},
	{			   "src_color",                renderer::blend_factor::src_color},
	{	 "one_minus_src_color",      renderer::blend_factor::one_minus_src_color},
	{			   "dst_color",                renderer::blend_factor::dst_color},
	{	 "one_minus_dst_color",      renderer::blend_factor::one_minus_dst_color},
	{			   "src_alpha",                renderer::blend_factor::src_alpha},
	{	 "one_minus_src_alpha",      renderer::blend_factor::one_minus_src_alpha},
	{			   "dst_alpha",                renderer::blend_factor::dst_alpha},
	{	 "one_minus_dst_alpha",      renderer::blend_factor::one_minus_dst_alpha},
	{		  "constant_color",           renderer::blend_factor::constant_color},
	{"one_minus_constant_color", renderer::blend_factor::one_minus_constant_color},
	{		  "constant_alpha",           renderer::blend_factor::constant_alpha},
	{"one_minus_constant_alpha", renderer::blend_factor::one_minus_constant_alpha},
	{	  "src_alpha_saturate",       renderer::blend_factor::src_alpha_saturate}
};

renderer::blend_factor blend_factor_from_string(const std::string& s)
{
	auto i = blend_factor_names.find(s);
	if (i == blend_factor_names.end()) {
		return renderer::blend_factor::one;
	}
	return i->second;
}
} // namespace

blending_widget::blending_widget(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
	widget(c, desc)
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "blend") {
			this->is_blending_enabled_v = get_property_value(p).to_bool();
		} else if (p.value == "blend_src") {
			this->blend_v.src = blend_factor_from_string(get_property_value(p).to_string());
		} else if (p.value == "blend_dst") {
			this->blend_v.dst = blend_factor_from_string(get_property_value(p).to_string());
		} else if (p.value == "blend_src_alpha") {
			this->blend_v.src_alpha = blend_factor_from_string(get_property_value(p).to_string());
		} else if (p.value == "blend_dst_alpha") {
			this->blend_v.dst_alpha = blend_factor_from_string(get_property_value(p).to_string());
		}
	}
}

void blending_widget::set_blending_to_renderer() const
{
	auto& r = this->context.get().renderer.get();
	r.set_blend_enabled(this->is_blending_enabled());
	if (this->is_blending_enabled()) {
		r.set_blend_func(this->blend_v.src, this->blend_v.dst, this->blend_v.src_alpha, this->blend_v.dst_alpha);
	}
}

void blending_widget::set_blending_enabled(bool enable)
{
	if (this->is_blending_enabled_v == enable) {
		return;
	}
	this->is_blending_enabled_v = enable;
	this->on_blending_change();
}

void blending_widget::set_blending_params(const blending_params& params)
{
	if (this->blend_v == params) {
		return;
	}
	this->blend_v = params;
	this->on_blending_change();
}
