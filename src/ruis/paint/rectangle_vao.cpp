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

#include "rectangle_vao.hpp"

#include <numbers>

#include <veg/canvas.hpp>

#include "../util/util.hpp"

using namespace ruis::paint;

std::map<
	ruis::paint::rectangle_vao::parameters, //
	std::weak_ptr<rectangle_vao::rounded_corners_texture>>
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, "false-positive")
	rectangle_vao::cache;

rectangle_vao::rectangle_vao(
	utki::shared_ref<const ruis::render::renderer> renderer, //
	parameters params
) :
	renderer(std::move(renderer)),
	params(params)
{
	this->update_rounded_corners_texture();
}

void rectangle_vao::set(parameters params)
{
	this->params = params;

	this->update_rounded_corners_texture();
}

void rectangle_vao::render(
	const ruis::mat4& matrix, //
	const vec2& dims,
	const ruis::color& color
) const
{
	auto& r = this->renderer.get();

	r.rendering_context.get().set_simple_alpha_blending();

	if (!this->rounded_corners_tex) {
		ruis::mat4 matr(matrix);
		matr.scale(dims);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			color.to_vec4f()
		);
	} else {
		this->render_rounded_corners(
			matrix, //
			dims,
			color
		);
	}
}

void rectangle_vao::render_rounded_corners(
	const mat4& matrix, //
	const vec2& dims,
	const ruis::color& color
) const
{
	auto& r = this->renderer.get();

	utki::assert(this->rounded_corners_tex);
	const auto& t = *this->rounded_corners_tex;
	const auto& rad = t.radii;

	const auto w = dims.x();
	const auto h = dims.y();
	const auto sw = this->params.stroke_width;
	const bool is_stroked = sw > 0;

	const auto& pos_quad = r.obj().pos_quad_01_vao.get();

	// The rounded rectangle is decomposed into:
	//  - four corner pieces, each rendered by its own VAO that samples only the arc
	//    of that corner (no straight parts of neighbouring corners);
	//  - four straight edge pieces;
	//  - a central piece (filled rectangles only).
	//
	// Filled: each edge piece is sized by the adjacent corner radius.
	// Stroked: each edge piece is sized by the stroke width.
	using std::max;

	// left-top corner
	if (rad.left_top() > 0) {
		ruis::mat4 m(matrix);
		m.scale(rad.left_top(), rad.left_top());
		r.shaders().color_pos_tex->render(m, t.vaos[0][0], color, t.tex);
	}

	// right-top corner
	if (rad.right_top() > 0) {
		ruis::mat4 m(matrix);
		m.translate(w - rad.right_top(), 0);
		m.scale(rad.right_top(), rad.right_top());
		r.shaders().color_pos_tex->render(m, t.vaos[0][1], color, t.tex);
	}

	// left-bottom corner
	if (rad.left_bottom() > 0) {
		ruis::mat4 m(matrix);
		m.translate(0, h - rad.left_bottom());
		m.scale(rad.left_bottom(), rad.left_bottom());
		r.shaders().color_pos_tex->render(m, t.vaos[1][0], color, t.tex);
	}

	// right-bottom corner
	if (rad.right_bottom() > 0) {
		ruis::mat4 m(matrix);
		m.translate(w - rad.right_bottom(), h - rad.right_bottom());
		m.scale(rad.right_bottom(), rad.right_bottom());
		r.shaders().color_pos_tex->render(m, t.vaos[1][1], color, t.tex);
	}

	// top edge: the strip between the two top corners; its height is the top-left
	// corner radius (the stroke width when stroked) so it lines up with that
	// corner's arc
	{
		auto size = vec2(
			max(w - rad.left_top() - rad.right_top(), real(0)), //
			is_stroked ? sw : rad.left_top()
		);
		if (size.x() > 0 && size.y() > 0) {
			ruis::mat4 m(matrix);
			m.translate(rad.left_top(), 0);
			m.scale(size);
			r.shaders().color_pos->render(m, pos_quad, color);
		}
	}

	// right edge: the strip between the two right corners; its width is the
	// top-right corner radius (the stroke width when stroked) so it lines up with
	// that corner's arc
	{
		auto size = vec2(
			is_stroked ? sw : rad.right_top(), //
			max(h - rad.right_top() - rad.right_bottom(), real(0))
		);
		if (size.x() > 0 && size.y() > 0) {
			ruis::mat4 m(matrix);
			m.translate(w - size.x(), rad.right_top());
			m.scale(size);
			r.shaders().color_pos->render(m, pos_quad, color);
		}
	}

	// bottom edge: the strip between the two bottom corners; its height is the
	// bottom-right corner radius (the stroke width when stroked) so it lines up
	// with that corner's arc
	{
		auto size = vec2(
			max(w - rad.left_bottom() - rad.right_bottom(), real(0)), //
			is_stroked ? sw : rad.right_bottom()
		);
		if (size.x() > 0 && size.y() > 0) {
			ruis::mat4 m(matrix);
			m.translate(rad.left_bottom(), h - size.y());
			m.scale(size);
			r.shaders().color_pos->render(m, pos_quad, color);
		}
	}

	// left edge: the strip between the two left corners; its width is the
	// bottom-left corner radius (the stroke width when stroked) so it lines up
	// with that corner's arc
	{
		auto size = vec2(
			is_stroked ? sw : rad.left_bottom(), //
			max(h - rad.left_top() - rad.left_bottom(), real(0))
		);
		if (size.x() > 0 && size.y() > 0) {
			ruis::mat4 m(matrix);
			m.translate(0, rad.left_top());
			m.scale(size);
			r.shaders().color_pos->render(m, pos_quad, color);
		}
	}

	// center: the inner rectangle filling the part not covered by the corner arcs
	// and the four edge strips (filled rectangles only)
	if (!is_stroked) {
		auto size = vec2(
			max(w - rad.left_bottom() - rad.right_top(), real(0)), //
			max(h - rad.left_top() - rad.right_bottom(), real(0))
		);
		if (size.x() > 0 && size.y() > 0) {
			ruis::mat4 m(matrix);
			m.translate(rad.left_bottom(), rad.left_top());
			m.scale(size);
			r.shaders().color_pos->render(m, pos_quad, color);
		}
	}
}

namespace {
// approximate 90 degree arc with bezier curve which matches the arc at 45 degree point
// and has the same tangent as an arc at 45 degree point
const auto arc_bezier_param = ruis::real(4 * (std::numbers::sqrt2 - 1) / 3);

void add_outer_roundend_corners_rectangle(
	veg::canvas& canvas, //
	const ruis::vec2& canvas_dims,
	const ruis::corners<ruis::real>& radii
)
{
	canvas.move_abs(ruis::vec2{
		0, //
		radii.left_top()
	});

	canvas.cubic_curve_rel(
		{0, -arc_bezier_param * radii.left_top()}, //
		{radii.left_top() * (1 - arc_bezier_param), -radii.left_top()},
		{radii.left_top(), -radii.left_top()}
	);

	canvas.line_abs(ruis::vec2(
		canvas_dims.x() - radii.right_top(), //
		0
	));

	canvas.cubic_curve_rel(
		{arc_bezier_param * radii.right_top(), 0}, //
		{radii.right_top(), radii.right_top() * (1 - arc_bezier_param)},
		{radii.right_top(), radii.right_top()}
	);

	canvas.line_abs(ruis::vec2(
		canvas_dims.x(), //
		canvas_dims.y() - radii.right_bottom()
	));

	canvas.cubic_curve_rel(
		{0, arc_bezier_param * radii.right_bottom()}, //
		{-radii.right_bottom() * (1 - arc_bezier_param), radii.right_bottom()},
		{-radii.right_bottom(), radii.right_bottom()}
	);

	canvas.line_abs(ruis::vec2(
		radii.left_bottom(), //
		canvas_dims.y()
	));

	canvas.cubic_curve_rel(
		{-arc_bezier_param * radii.left_bottom(), 0}, //
		{-radii.left_bottom(), -radii.left_bottom() * (1 - arc_bezier_param)},
		{-radii.left_bottom(), -radii.left_bottom()}
	);

	canvas.close_path();
}

void add_inner_roundend_corners_rectangle(
	veg::canvas& canvas, //
	const ruis::vec2& canvas_dims,
	const ruis::corners<ruis::real>& radii,
	ruis::real stroke_width
)
{
	utki::assert(stroke_width > 0);

	using std::max;

	ruis::corners<ruis::real> inner_radii = radii;
	for (auto& r : inner_radii) {
		r -= stroke_width;
	}

	canvas.move_abs(ruis::vec2{
		stroke_width, //
		max(radii.left_top(), stroke_width)
	});

	if (inner_radii.left_top() > 0) {
		canvas.cubic_curve_rel(
			{0, -arc_bezier_param * inner_radii.left_top()}, //
			{inner_radii.left_top() * (1 - arc_bezier_param), -inner_radii.left_top()},
			{inner_radii.left_top(), -inner_radii.left_top()}
		);
	}

	canvas.line_abs(ruis::vec2(
		canvas_dims.x() - max(radii.right_top(), stroke_width), //
		stroke_width
	));

	if (inner_radii.right_top() > 0) {
		canvas.cubic_curve_rel(
			{arc_bezier_param * inner_radii.right_top(), 0}, //
			{inner_radii.right_top(), inner_radii.right_top() * (1 - arc_bezier_param)},
			{inner_radii.right_top(), inner_radii.right_top()}
		);
	}

	canvas.line_abs(ruis::vec2(
		canvas_dims.x() - stroke_width, //
		canvas_dims.y() - max(radii.right_bottom(), stroke_width)
	));

	if (inner_radii.right_bottom() > 0) {
		canvas.cubic_curve_rel(
			{0, arc_bezier_param * inner_radii.right_bottom()}, //
			{-inner_radii.right_bottom() * (1 - arc_bezier_param), inner_radii.right_bottom()},
			{-inner_radii.right_bottom(), inner_radii.right_bottom()}
		);
	}

	canvas.line_abs(ruis::vec2(
		max(radii.left_bottom(), stroke_width), //
		canvas_dims.y() - stroke_width
	));

	if (inner_radii.left_bottom() > 0) {
		canvas.cubic_curve_rel(
			{-arc_bezier_param * inner_radii.left_bottom(), 0}, //
			{-inner_radii.left_bottom(), -inner_radii.left_bottom() * (1 - arc_bezier_param)},
			{-inner_radii.left_bottom(), -inner_radii.left_bottom()}
		);
	}

	canvas.close_path();
}

auto make_rounded_corners_texture_image(const rectangle_vao::parameters& params)
{
	const auto& radii = params.corner_radii;

	using std::max;
	auto canvas_dims = ruis::vec2(
		max({radii.left_top(), radii.left_bottom(), params.stroke_width}) +
			max({radii.right_top(), radii.right_bottom(), params.stroke_width}), //
		max({radii.left_top(), radii.right_top(), params.stroke_width}) +
			max({radii.left_bottom(), radii.right_bottom(), params.stroke_width})
	);

	veg::canvas canvas(canvas_dims.to<uint32_t>());

	add_outer_roundend_corners_rectangle(
		canvas, //
		canvas_dims,
		radii
	);

	if (params.stroke_width > 0) {
		// stroked rectangle, so we need to add inner rectangle to make it hollow
		add_inner_roundend_corners_rectangle(
			canvas, //
			canvas_dims,
			radii,
			params.stroke_width
		);
	}

	// white
	canvas.set_source({1, 1, 1, 1});

	canvas.fill();

	return canvas.release();
}

utki::shared_ref<const ruis::render::vertex_array> make_quad_vao(
	const ruis::render::renderer& ren, //
	const r4::segment2<ruis::real>& tex_coords
)
{
	std::array<ruis::vec2, 4> tex_coords_array = {
		tex_coords.p1, //
		tex_coords.x1_y2(),
		tex_coords.p2,
		tex_coords.x2_y1()
	};

	return ren.ctx().make_vertex_array(
		// clang-format off
		{
			ren.obj().quad_01_vbo,
			ren.ctx().make_vertex_buffer(tex_coords_array)
		},
		// clang-format on
		ren.obj().quad_fan_indices,
		ruis::render::vertex_array::mode::triangle_fan
	);
}

std::array<std::array<utki::shared_ref<const ruis::render::vertex_array>, 2>, 2> make_corner_vaos(
	const ruis::render::renderer& ren, //
	const ruis::vec2& tex_dims,
	const ruis::corners<ruis::real>& radii
)
{
	auto tw = tex_dims.x();
	auto th = tex_dims.y();

	// The composite texture holds the four corner arcs in its four corners, each
	// arc in a bounding square of its own radius size. Each corner VAO samples
	// only that square, so no straight parts of neighbouring corners are included.
	// clang-format off
	return {{
		{
			// left-top
			make_quad_vao(ren,
				{
					.p1 = {0, 0},
					.p2 = {radii.left_top() / tw, radii.left_top() / th}
				}
			),
			// right-top
			make_quad_vao(ren,
				{
					.p1 = {1 - radii.right_top() / tw, 0},
					.p2 = {1, radii.right_top() / th}
				}
			)
		},
		{
			// left-bottom
			make_quad_vao(ren,
				{
					.p1 = {0, 1 - radii.left_bottom() / th},
					.p2 = {radii.left_bottom() / tw, 1}
				}
			),
			// right-bottom
			make_quad_vao(ren,
				{
					.p1 = {1 - radii.right_bottom() / tw, 1 - radii.right_bottom() / th},
					.p2 = {1, 1}
				}
			)
		}
	}};
	// clang-format on
}

} // namespace

rectangle_vao::rounded_corners_texture::rounded_corners_texture(
	const ruis::render::renderer& r, //
	utki::shared_ref<const render::texture_2d> tex,
	corners<real> radii
) :
	tex(std::move(tex)),
	radii(std::move(radii)),
	vaos(make_corner_vaos(
		r, //
		this->tex.get().dims().to<real>(),
		this->radii
	))
{}

void rectangle_vao::update_rounded_corners_texture()
{
	if (this->params.corner_radii.is_zero()) {
		this->rounded_corners_tex.reset();
		return;
	}

	// TODO: develop algorithm to go through cache from time to time and drop zombie textures
	if (auto i = this->cache.find(this->params); i != this->cache.end()) {
		if (auto t = i->second.lock()) {
			this->rounded_corners_tex = std::move(t);
			return;
		} else {
			this->cache.erase(i);
		}
	}

	auto raster_image = make_rounded_corners_texture_image(this->params);

	// TODO: convert to greyscale image

	const auto& r = this->renderer.get();

	auto tex = r.ctx().make_texture_2d(std::move(raster_image), {});

	this->rounded_corners_tex = std::make_shared<rounded_corners_texture>(
		r, //
		std::move(tex),
		this->params.corner_radii //
	);

	// add to cache
	this->cache.insert(std::make_pair(
		this->params, //
		utki::make_weak(this->rounded_corners_tex)
	));
}
