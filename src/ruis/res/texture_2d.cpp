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

#include "texture_2d.hpp"

#include "../context.hpp"
#include "../util/util.hpp"

using namespace std::string_view_literals;

using namespace ruis;
using namespace ruis::res;

namespace {
render::texture_2d::filter parse_filter(std::string_view str)
{
	if (str == "nearest"sv) {
		return render::texture_2d::filter::nearest;
	} else if (str == "linear"sv) {
		return render::texture_2d::filter::linear;
	}
	throw std::invalid_argument(
		utki::cat("res::texture: unknown filter value specified in the resource description: ", str)
	);
}
} // namespace

namespace {
render::texture_2d::mipmap parse_mipmap(std::string_view str)
{
	if (str == "none"sv) {
		return render::texture_2d::mipmap::none;
	} else if (str == "nearest"sv) {
		return render::texture_2d::mipmap::nearest;
	} else if (str == "linear"sv) {
		return render::texture_2d::mipmap::linear;
	}
	throw std::invalid_argument(
		utki::cat("res::texture: unknown mipmap value specified in the resource description: ", str)
	);
}
} // namespace

texture_2d::texture_2d(utki::shared_ref<const render::texture_2d> tex) :
	tex2d(std::move(tex))
{}

utki::shared_ref<texture_2d> texture_2d::load(
	ruis::resource_loader& loader,
	const tml::forest& desc,
	const papki::file& fi
)
{
	ruis::render::context::texture_2d_parameters params{
		.min_filter = render::texture_2d::filter::linear,
		.mag_filter = render::texture_2d::filter::linear,
		.mipmap = render::texture_2d::mipmap::nearest
	};

	for (auto& p : desc) {
		if (p.value == "file"sv) {
			fi.set_path(get_property_value(p).string);
		} else if (p.value == "min_filter"sv) {
			params.min_filter = parse_filter(get_property_value(p).string);
		} else if (p.value == "mag_filter"sv) {
			params.mag_filter = parse_filter(get_property_value(p).string);
		} else if (p.value == "mipmap"sv) {
			params.mipmap = parse_mipmap(get_property_value(p).string);
		}
	}

	auto tex = loader.renderer.get().render_context.get().create_texture_2d( //
		rasterimage::read(fi),
		std::move(params)
	);

	return utki::make_shared<texture_2d>( //
		std::move(tex)
	);
}
