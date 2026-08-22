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

#include "oval_vao.hpp"

#include <veg/canvas.hpp>

using namespace ruis::paint;

std::map<
	ruis::vec2, // radii
	std::weak_ptr<const ruis::render::texture_2d>>
	// TODO: ? NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, "false-positive")
	oval_vao::cache;

oval_vao::oval_vao(
	utki::shared_ref<const ruis::render::renderer> renderer,
	vec2 radii, //
	real stroke_width
) :
	renderer(std::move(renderer))
{
	this->set(
		radii, //
		stroke_width
	);
}

void oval_vao::set(
	vec2 radii, //
	real stroke_width
)
{
	this->radii = radii;
	this->stroke_width = stroke_width;

	this->update_texture();
}

namespace {
auto make_oval_texture_image(ruis::vec2 radii)
{
	using std::round;
	veg::canvas canvas(round(radii * 2).to<uint32_t>());

	canvas.ellipse(radii, radii);

	// white
	canvas.set_source({1, 1, 1, 1});

	canvas.fill();

	return canvas.release();
}
} // namespace

void oval_vao::update_texture()
{
	if (this->radii.is_zero()) {
		this->tex.reset();
		return;
	}

	// try to find in cache
	// TODO: develop algorithm to go through cache from time to time and drop zombie textures
	auto it = this->cache.find(this->radii);
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
		make_oval_texture_image(this->radii), //
		{}
	);

	// add to cache
	this->cache.insert(std::make_pair(
		this->radii, //
		utki::make_weak(this->tex)
	));
}

void oval_vao::render(
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
	matr.scale(this->radii * 2);

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
