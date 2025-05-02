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

#include "nine_patch.hpp"

#include <iomanip>

#include "../context.hpp"
#include "../render/vertex_array.hpp"
#include "../util/util.hpp"

using namespace ruis;
using namespace ruis::res;

utki::shared_ref<nine_patch> nine_patch::load(
	const ruis::resource_loader& loader, //
	const tml::forest& desc,
	const papki::file& fi
)
{
	sides<real> fraction_borders(-1);
	for (auto& p : desc) {
		if (p.value == "borders") {
			fraction_borders = make_sides(p.children);
		} else if (p.value == "file") {
			fi.set_path(get_property_value(p).string);
		}
	}
	if (fraction_borders.left() < 0) {
		throw std::invalid_argument("nine_patch::load(): could not read borders");
	}

	for (const auto& b : fraction_borders) {
		if (b < 0 || 1 < b) {
			throw std::invalid_argument("nine_patch::load(): one of the borders is out of [0:1] range");
		}
	}

	auto image = res::image::load(
		loader, //
		fi
	);

	return utki::make_shared<nine_patch>( //
		loader.renderer,
		std::move(image),
		fraction_borders
	);
}

namespace {
utki::shared_ref<const ruis::render::vertex_array> make_quad_vao(
	ruis::render::renderer& r, //
	utki::span<const vec2> quad_tex_coords
)
{
	return r.ctx().make_vertex_array(
		// clang-format off
		{
			r.obj().quad_01_vbo,
			r.ctx().make_vertex_buffer(quad_tex_coords)
		},
		// clang-format on
		r.obj().quad_fan_indices,
		ruis::render::vertex_array::mode::triangle_fan
	);
}
} // namespace

nine_patch::nine_patch(
	utki::shared_ref<ruis::render::renderer> renderer, //
	utki::shared_ref<const res::image> image,
	sides<real> fraction_borders
) :
	renderer(std::move(renderer)),
	// clang-format off
	vaos{{
		{
			make_quad_vao(this->renderer, utki::span<const vec2>({
				vec2(0, 0),
				vec2(0, fraction_borders.top()),
				fraction_borders.left_top(),
				vec2(fraction_borders.left(), 0)
			})),
			make_quad_vao(this->renderer, utki::span<const vec2>({
				vec2(fraction_borders.left(), 0),
				fraction_borders.left_top(),
				vec2(real(1) - fraction_borders.right(), fraction_borders.top()),
				vec2(real(1) - fraction_borders.right(), 0)
			})),
			make_quad_vao(this->renderer, utki::span<const vec2>({
				vec2(real(1) - fraction_borders.right(), 0),
				vec2(real(1) - fraction_borders.right(), fraction_borders.top()),
				vec2(real(1), fraction_borders.top()),
				vec2(real(1), 0)
			}))
		},
		{
			make_quad_vao(this->renderer, utki::span<const vec2>({
				vec2(0, fraction_borders.top()),
				vec2(0, real(1) - fraction_borders.bottom()),
				vec2(fraction_borders.left(), real(1) - fraction_borders.bottom()),
				vec2(fraction_borders.left(), fraction_borders.top()),
			})),
			make_quad_vao(this->renderer, utki::span<const vec2>({
				fraction_borders.left_top(),
				vec2(fraction_borders.left(), real(1) - fraction_borders.bottom()),
				vec2(real(1) - fraction_borders.right(), real(1) - fraction_borders.bottom()),
				vec2(real(1) - fraction_borders.right(), fraction_borders.top())
			})),
			make_quad_vao(this->renderer, utki::span<const vec2>({
				vec2(real(1) - fraction_borders.right(), fraction_borders.top()),
				vec2(real(1) - fraction_borders.right(), real(1) - fraction_borders.bottom()),
				vec2(real(1), real(1) - fraction_borders.bottom()),
				vec2(real(1), fraction_borders.top()),
			}))
		},
		{
			make_quad_vao(this->renderer, utki::span<const vec2>({
				vec2(0, real(1) - fraction_borders.bottom()),
				vec2(0, real(1)),
				vec2(fraction_borders.left(), real(1)),
				vec2(fraction_borders.left(), real(1) - fraction_borders.bottom()),
			})),
			make_quad_vao(this->renderer, utki::span<const vec2>({
				vec2(fraction_borders.left(), real(1) - fraction_borders.bottom()),
				vec2(fraction_borders.left(), real(1)),
				vec2(real(1) - fraction_borders.right(), real(1)),
				vec2(real(1) - fraction_borders.right(), real(1) - fraction_borders.bottom()),
			})),
			make_quad_vao(this->renderer, utki::span<const vec2>({
				vec2(real(1) - fraction_borders.right(), real(1) - fraction_borders.bottom()),
				vec2(real(1) - fraction_borders.right(), real(1)),
				vec2(1, 1),
				vec2(1, real(1) - fraction_borders.bottom())
			}))
		}
	}},
	// clang-format on
	image(std::move(image)), fraction_borders(fraction_borders)
{}

sides<real> nine_patch::get_borders(const ruis::units& units) const noexcept
{
	auto dims = this->image.get().dims(units).to<real>();

	using std::round;

	return {
		round(this->fraction_borders.left() * dims.x()),
		round(this->fraction_borders.top() * dims.y()),
		round(this->fraction_borders.right() * dims.x()),
		round(this->fraction_borders.bottom() * dims.y())
	};
}
