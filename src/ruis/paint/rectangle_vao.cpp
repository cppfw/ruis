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
	utki::shared_ref<const ruis::render::renderer> renderer,
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

	const auto& tex_middle = t.middle_px;
	auto tex_dims = t.tex.get().dims().to<real>();
	auto tex_tail = tex_dims - tex_middle;
	auto center_size = dims - tex_dims;
	auto tail_pos = dims - tex_tail;

	// left-top
	{
		ruis::mat4 matr(matrix);
		matr.scale(tex_middle);

		r.shaders().color_pos_tex->render(
			matr, //
			t.vaos[0][0],
			color,
			t.tex
		);
	}

	// right-top
	{
		ruis::mat4 matr(matrix);
		matr.translate(
			tail_pos.x(), //
			0
		);
		matr.scale(
			tex_tail.x(), //
			tex_middle.y()
		);

		r.shaders().color_pos_tex->render(
			matr, //
			t.vaos[0][1],
			color,
			t.tex
		);
	}

	// left-bottom
	{
		ruis::mat4 matr(matrix);
		matr.translate(
			0, //
			tail_pos.y()
		);
		matr.scale(
			tex_middle.x(), //
			tex_tail.y()
		);

		r.shaders().color_pos_tex->render(
			matr, //
			t.vaos[1][0],
			color,
			t.tex
		);
	}

	// right-bottom
	{
		ruis::mat4 matr(matrix);

		matr.translate(tail_pos);
		matr.scale(tex_tail);

		r.shaders().color_pos_tex->render(
			matr, //
			t.vaos[1][1],
			color,
			t.tex
		);
	}

	bool is_stroked = this->params.stroke_width > 0;

	// top
	{
		ruis::mat4 matr(matrix);
		matr.translate(
			tex_middle.x(), //
			0
		);
		matr.scale(
			center_size.x(), //
			is_stroked ? this->params.stroke_width : tex_middle.y()
		);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			color
		);
	}

	// left
	{
		ruis::mat4 matr(matrix);
		matr.translate(
			0, //
			tex_middle.y()
		);
		matr.scale(
			is_stroked ? this->params.stroke_width : tex_middle.x(), //
			center_size.y()
		);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			color
		);
	}

	// center
	if(!is_stroked){
		ruis::mat4 matr(matrix);
		matr.translate(tex_middle);
		matr.scale(center_size);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			color
		);
	}

	// right
	{
		ruis::mat4 matr(matrix);
		if(!is_stroked){
			// filled rectangle
			matr.translate(
				tail_pos.x(), //
				tex_middle.y()
			);
			matr.scale(
				tex_tail.x(), //
				center_size.y()
			);
		}else{
			// stroked rectangle
			matr.translate(
				dims.x() - this->params.stroke_width, //
				tex_middle.y()
			);
			matr.scale(
				this->params.stroke_width, //
				center_size.y()
			);
		}

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			color
		);
	}

	// bottom
	{
		ruis::mat4 matr(matrix);
		if(!is_stroked){
			// filled rectangle
			matr.translate(
				tex_middle.x(), //
				tail_pos.y()
			);
			matr.scale(
				center_size.x(), //
				tex_tail.y()
			);
		}else{
			// stroked rectangle
			matr.translate(
				tex_middle.x(), //
				dims.y() - this->params.stroke_width
			);
			matr.scale(
				center_size.x(), //
				this->params.stroke_width
			);
		}

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			color
		);
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
	for(auto& r : inner_radii){
		r -= stroke_width;
	}

	canvas.move_abs(ruis::vec2{
		stroke_width, //
		max(radii.left_top(), stroke_width)
	});

	if(inner_radii.left_top() > 0){
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

	if(inner_radii.right_top() > 0){
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

	if(inner_radii.right_bottom() > 0){
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

	if(inner_radii.left_bottom() > 0){
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
		max({radii.left_top(), radii.left_bottom(), params.stroke_width}) + max({radii.right_top(), radii.right_bottom(), params.stroke_width}), //
		max({radii.left_top(), radii.right_top(), params.stroke_width}) + max({radii.left_bottom(), radii.right_bottom(), params.stroke_width})
	);

	veg::canvas canvas(canvas_dims.to<uint32_t>());

	add_outer_roundend_corners_rectangle(
		canvas, //
		canvas_dims,
		radii
	);

	if(params.stroke_width > 0){
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

} // namespace

rectangle_vao::rounded_corners_texture::rounded_corners_texture(
	const ruis::render::renderer& r, //
	utki::shared_ref<const render::texture_2d> tex,
	vec2 middle_px
) :
	rounded_corners_texture(
		r, //
		middle_px.comp_div(tex.get().dims().to<real>()),
		// Arguments evaluation order is unspecified, so we cannot do std::move(tex) here,
		// because we also use it in another argument.
		// So, we pass it by reference to avoid unneded shared_ref copying.
		tex,
		middle_px
	)
{}

rectangle_vao::rounded_corners_texture::rounded_corners_texture(
	const ruis::render::renderer& r, //
	vec2 middle,
	utki::shared_ref<const render::texture_2d>& tex,
	vec2 middle_px
) :
	tex(std::move(tex)),
	middle_px(middle_px),
	// clang-format off
	vaos{{
		{
			make_quad_vao(r,
				{
					.p1 = {0, 0},
					.p2 = middle
				}
			),
			make_quad_vao(r,
				{
					.p1 = {
						middle.x(),
						0
					},
					.p2 = {
						1,
						middle.y()
					}
				}
			)
		},
		{
			make_quad_vao(r,
				{
					.p1 = {
						0,
						middle.y()
					},
					.p2 = {
						middle.x(),
						1
					}
				}
			),
			make_quad_vao(r,
				{
					.p1 = middle,
					.p2 = {1, 1}
				}
			)
		}
	}}
// clang-format on
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

	using std::max;
	auto middle_px = ruis::vec2(
		max(this->params.corner_radii[0], this->params.corner_radii[3]),
		max(this->params.corner_radii[0], this->params.corner_radii[1]) //
	);

	this->rounded_corners_tex = std::make_shared<rounded_corners_texture>(
		r, //
		std::move(tex),
		middle_px
	);

	// add to cache
	this->cache.insert(std::make_pair(
		this->params, //
		utki::make_weak(this->rounded_corners_tex)
	));
}
