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

#include "ellipse_vao.hpp"

#include <veg/canvas.hpp>

using namespace ruis::paint;

std::map<
	ruis::vec2, // diameters
	std::weak_ptr<const ruis::render::texture_2d>>
	// TODO: ? NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, "false-positive")
	ellipse_vao::cache;

ellipse_vao::ellipse_vao(
	utki::shared_ref<const ruis::render::renderer> renderer,
	vec2 diameters, //
	real stroke_width
) :
	renderer(std::move(renderer))
{
	this->set(
		diameters, //
		stroke_width
	);
}

void ellipse_vao::set(
	vec2 diameters, //
	real stroke_width
)
{
	this->diameters = diameters;
	this->stroke_width = stroke_width;

	this->update_texture();
}

namespace {
auto make_ellipse_texture_image(ruis::vec2 diameters)
{
	using std::round;
	veg::canvas canvas(round(diameters).to<uint32_t>());

	canvas.ellipse(
		diameters / 2, // center
		diameters / 2 // radii
	);

	// white
	canvas.set_source({1, 1, 1, 1});

	canvas.fill();

	return canvas.release();
}
} // namespace

void ellipse_vao::update_texture()
{
	if (!this->diameters.is_positive()) {
		this->tex.reset();
		return;
	}

	// try to find in cache
	// TODO: develop algorithm to go through cache from time to time and drop zombie textures
	auto it = this->cache.find(this->diameters);
	if (it != this->cache.end()) {
		if (auto t = it->second.lock()) {
			this->tex = std::move(t);
			return;
		} else {
			this->cache.erase(it);
		}
	}

	// TODO: convert to greyscale image

	const auto& r = this->renderer.get();

	this->tex = r.ctx().make_texture_2d(
		make_ellipse_texture_image(this->diameters), //
		{}
	);

	// add to cache
	this->cache.insert(std::make_pair(
		this->diameters, //
		utki::make_weak(this->tex)
	));
}

void ellipse_vao::render(
	const ruis::mat4& matrix, //
	const ruis::color& color
) const
{
	if (!this->tex) {
		return;
	}

	auto& r = this->renderer.get();

	r.rendering_context.get().set_simple_alpha_blending();

	ruis::mat4 matr(matrix);
	matr.scale(this->diameters);

	r.shaders().color_pos_tex->render(
		matr, //
		r.obj().pos_tex_quad_01_vao.get(),
		color,
		*this->tex
	);

	// r.shaders().color_pos->render(
	//     matr, //
	//     r.obj().pos_quad_01_vao.get(),
	//     color
	// );
}
