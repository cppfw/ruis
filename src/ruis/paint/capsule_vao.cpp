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

#include "capsule_vao.hpp"

#include <numbers>

#include <veg/canvas.hpp>

using namespace ruis::paint;

std::map<
	ruis::real, // diameter
	std::weak_ptr<const ruis::render::texture_2d>>
	// TODO: ? NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, "false-positive")
	capsule_vao::cache;

capsule_vao::capsule_vao(
	utki::shared_ref<const ruis::render::renderer> renderer, //
	ruis::real diameter,
	ruis::real stroke_width
) :
	renderer(std::move(renderer))
{
	this->set(
		diameter, //
		stroke_width
	);
}

void capsule_vao::set(
	ruis::real diameter, //
	ruis::real stroke_width
)
{
	this->diameter = diameter;
	this->stroke_width = stroke_width;

	this->update_texture();
}

namespace {
auto make_capsule_texture_image(
	ruis::real diameter, //
	ruis::real stroke_width
)
{
	auto canvas_size = round(ruis::vec2(diameter)).to<uint32_t>();

	veg::canvas canvas(canvas_size);

	auto radius = diameter / 2;
	auto center = r4::vector2<ruis::real>{radius};

	canvas.circle(center, radius);

	if (stroke_width > 0) {
		using std::max;
		auto inner_radius =
			max(radius - stroke_width, //
				ruis::real(0) // clamp bottom to 0
			);

		canvas.circle(center, inner_radius);
	}

	// white
	canvas.set_source({1, 1, 1, 1});

	canvas.fill();

	return canvas.release();
}
} // namespace

void capsule_vao::update_texture()
{
	if (this->diameter <= 0) {
		this->tex.reset();
		return;
	}

	// try to find in cache
	// TODO: develop algorithm to go through cache from time to time and drop zombie textures
	auto it = this->cache.find(this->diameter);
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
		make_capsule_texture_image(
			this->diameter, //
			this->stroke_width
		), //
		{}
	);

	// add to cache
	this->cache.insert(std::make_pair(
		this->diameter, //
		utki::make_weak(this->tex)
	));
}

void capsule_vao::render(
	const ruis::mat4& matrix, //
	const ruis::color& color,
	ruis::real length,
	bool is_vertical
) const
{
	if (!this->tex) {
		return;
	}

	auto& r = this->renderer.get();

	r.rendering_context.get().set_simple_alpha_blending();

	ruis::real radius = this->diameter / 2;

	ruis::real tail_pos = length - radius;
	ruis::real center_length = length - this->diameter;

	// TODO: clamp tial to radius
	// TODO: clamp center to 0

	if (is_vertical) {
		// top-left
		{
			ruis::mat4 matr(matrix);
			matr.scale(radius, radius);

			r.shaders().color_pos_tex->render(
				matr, //
				r.obj().pos_tex_texture_quadrants_01_vaos[0].get(),
				color,
				*this->tex
			);
		}

		// top-right
		{
			ruis::mat4 matr(matrix);
			matr.translate(radius, 0);
			matr.scale(radius, radius);

			r.shaders().color_pos_tex->render(
				matr, //
				r.obj().pos_tex_texture_quadrants_01_vaos[1].get(),
				color,
				*this->tex
			);
		}

		// middle
		if (this->stroke_width <= 0) {
			ruis::mat4 matr(matrix);
			matr.translate(0, radius);
			matr.scale(this->diameter, center_length);

			r.shaders().color_pos->render(
				matr, //
				r.obj().pos_quad_01_vao.get(),
				color
			);
		} else {
			// left
			{
				ruis::mat4 matr(matrix);
				matr.translate(0, radius);
				matr.scale(this->stroke_width, center_length);

				r.shaders().color_pos->render(
					matr, //
					r.obj().pos_quad_01_vao.get(),
					color
				);
			}

			// right
			{
				ruis::mat4 matr(matrix);
				matr.translate(this->diameter - this->stroke_width, radius);
				matr.scale(this->stroke_width, center_length);

				r.shaders().color_pos->render(
					matr, //
					r.obj().pos_quad_01_vao.get(),
					color
				);
			}
		}

		// bottom-right
		{
			ruis::mat4 matr(matrix);
			matr.translate(radius, tail_pos);
			matr.scale(radius, radius);

			r.shaders().color_pos_tex->render(
				matr, //
				r.obj().pos_tex_texture_quadrants_01_vaos[2].get(),
				color,
				*this->tex
			);
		}

		// bottom-left
		{
			ruis::mat4 matr(matrix);
			matr.translate(0, tail_pos);
			matr.scale(radius, radius);

			r.shaders().color_pos_tex->render(
				matr, //
				r.obj().pos_tex_texture_quadrants_01_vaos[3].get(),
				color,
				*this->tex
			);
		}
	} else {
		// top-left
		{
			ruis::mat4 matr(matrix);
			matr.scale(radius, radius);

			r.shaders().color_pos_tex->render(
				matr, //
				r.obj().pos_tex_texture_quadrants_01_vaos[0].get(),
				color,
				*this->tex
			);
		}

		// top-right
		{
			ruis::mat4 matr(matrix);
			matr.translate(tail_pos, 0);
			matr.scale(radius, radius);

			r.shaders().color_pos_tex->render(
				matr, //
				r.obj().pos_tex_texture_quadrants_01_vaos[1].get(),
				color,
				*this->tex
			);
		}

		// middle
		if (this->stroke_width <= 0) {
			ruis::mat4 matr(matrix);
			matr.translate(radius, 0);
			matr.scale(center_length, this->diameter);

			r.shaders().color_pos->render(
				matr, //
				r.obj().pos_quad_01_vao.get(),
				color
			);
		} else {
			// top
			{
				ruis::mat4 matr(matrix);
				matr.translate(radius, 0);
				matr.scale(center_length, this->stroke_width);

				r.shaders().color_pos->render(
					matr, //
					r.obj().pos_quad_01_vao.get(),
					color
				);
			}

			// bottom
			{
				ruis::mat4 matr(matrix);
				matr.translate(radius, this->diameter - this->stroke_width);
				matr.scale(center_length, this->stroke_width);

				r.shaders().color_pos->render(
					matr, //
					r.obj().pos_quad_01_vao.get(),
					color
				);
			}
		}

		// bottom-right
		{
			ruis::mat4 matr(matrix);
			matr.translate(tail_pos, radius);
			matr.scale(radius, radius);

			r.shaders().color_pos_tex->render(
				matr, //
				r.obj().pos_tex_texture_quadrants_01_vaos[2].get(),
				color,
				*this->tex
			);
		}

		// bottom-left
		{
			ruis::mat4 matr(matrix);
			matr.translate(0, radius);
			matr.scale(radius, radius);

			r.shaders().color_pos_tex->render(
				matr, //
				r.obj().pos_tex_texture_quadrants_01_vaos[3].get(),
				color,
				*this->tex
			);
		}
	}
}
