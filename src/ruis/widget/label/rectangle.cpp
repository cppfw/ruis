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

#include "rectangle.hpp"

#include <veg/canvas.hpp>

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace ruis;

std::map<
	sides<real>, //
	std::weak_ptr<rectangle::rounded_corners_texture>>
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, "false-positive")
	rectangle::cache;

rectangle::rectangle( //
	utki::shared_ref<ruis::context> context,
	all_parameters params,
	widget_list children
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	// clang-format off
	padding( //
		this->context,
		{
			.container_params = std::move(params.container_params),
			.padding_params = std::move(params.padding_params)
		},
		std::move(children)
	),
	// clang-format on
	color_widget( //
		this->context,
		std::move(params.color_params)
	),
	params(std::move(params.rectangle_params))
{
	this->update_rounded_corners_texture();
}

void rectangle::render(const ruis::matrix4& matrix) const
{
	auto& r = this->context.get().renderer.get();

	r.rendering_context.get().set_simple_alpha_blending();

	if (!this->rounded_corners_tex) {
		ruis::matrix4 matr(matrix);
		matr.scale(this->rect().d);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			this->get_current_color().to_vec4f()
		);
	} else {
		this->render_rounder_corners(matrix);
	}

	this->padding::render(matrix);
}

void rectangle::render_rounder_corners(const mat4& matrix) const
{
	auto& r = this->context.get().renderer.get();

	color clr = this->get_current_color();

	ASSERT(this->rounded_corners_tex)
	const auto& t = *this->rounded_corners_tex;

	const auto& tex_middle = t.middle_px;
	auto tex_dims = t.tex.get().dims().to<real>();
	auto tex_tail = tex_dims - tex_middle;
	auto center_size = this->rect().d - tex_dims;
	auto tail_pos = this->rect().d - tex_tail;

	// left-top
	{
		ruis::mat4 matr(matrix);
		matr.scale(tex_middle);

		r.shaders().color_pos_tex->render(
			matr, //
			t.vaos[0][0],
			clr,
			t.tex
		);
	}

	// top
	{
		ruis::mat4 matr(matrix);
		matr.translate(
			tex_middle.x(), //
			0
		);
		matr.scale(
			center_size.x(), //
			tex_middle.y()
		);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			clr
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
			clr,
			t.tex
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
			tex_middle.x(), //
			center_size.y()
		);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			clr
		);
	}

	// center
	{
		ruis::mat4 matr(matrix);
		matr.translate(tex_middle);
		matr.scale(center_size);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			clr
		);
	}

	// right
	{
		ruis::mat4 matr(matrix);
		matr.translate(
			tail_pos.x(), //
			tex_middle.y()
		);
		matr.scale(
			tex_tail.x(), //
			center_size.y()
		);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			clr
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
			clr,
			t.tex
		);
	}

	// bottom
	{
		ruis::mat4 matr(matrix);
		matr.translate(
			tex_middle.x(), //
			tail_pos.y()
		);
		matr.scale(
			center_size.x(), //
			tex_tail.y()
		);

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			clr
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
			clr,
			t.tex
		);
	}
}

namespace {
// approximate 90 degree arc with bezier curve which matches the arc at 45 degree point
// and has the same tangent as an arc at 45 degree point
using std::sqrt;
const real arc_bezier_param = real(4 * (sqrt(2) - 1) / 3);

auto make_rounded_corners_texture_image(const sides<real>& radii)
{
	const auto& left_top = radii[0];
	const auto& right_top = radii[1];
	const auto& right_bottom = radii[2];
	const auto& left_bottom = radii[3];

	using std::max;
	auto canvas_size = vec2(
		max(left_top, left_bottom) + max(right_top, right_bottom), //
		max(left_top, right_top) + max(left_bottom, right_bottom)
	);

	veg::canvas canvas(canvas_size.to<uint32_t>());

	canvas.move_abs(vec2{
		0, //
		left_top
	});

	canvas.cubic_curve_rel(
		{0, -arc_bezier_param * left_top}, //
		{left_top * (1 - arc_bezier_param), -left_top},
		{left_top, -left_top}
	);

	canvas.line_abs(vec2(
		canvas_size.x() - right_top, //
		0
	));

	canvas.cubic_curve_rel(
		{arc_bezier_param * right_top, 0}, //
		{right_top, right_top * (1 - arc_bezier_param)},
		{right_top, right_top}
	);

	canvas.line_abs(vec2(
		canvas_size.x(), //
		canvas_size.y() - right_bottom
	));

	canvas.cubic_curve_rel(
		{0, arc_bezier_param * right_bottom}, //
		{-right_bottom * (1 - arc_bezier_param), right_bottom},
		{-right_bottom, right_bottom}
	);

	canvas.line_abs(vec2(
		left_bottom, //
		canvas_size.y()
	));

	canvas.cubic_curve_rel(
		{-arc_bezier_param * left_bottom, 0}, //
		{-left_bottom, -left_bottom * (1 - arc_bezier_param)},
		{-left_bottom, -left_bottom}
	);

	canvas.close_path();

	// white
	canvas.set_source({1, 1, 1, 1});

	canvas.fill();

	return canvas.release();
}

utki::shared_ref<const ruis::render::vertex_array> make_quad_vao(
	ruis::render::renderer& ren, //
	const r4::segment2<real>& tex_coords
)
{
	std::array<vec2, 4> tex_coords_array = {
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

rectangle::rounded_corners_texture::rounded_corners_texture(
	ruis::render::renderer& r, //
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

rectangle::rounded_corners_texture::rounded_corners_texture(
	ruis::render::renderer& r, //
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
					{0, 0},
					middle
				}
			),
			make_quad_vao(r,
				{
					{
						middle.x(),
						0
					},
					{
						1,
						middle.y()
					}
				}
			)
		},
		{
			make_quad_vao(r,
				{
					{
						0,
						middle.y()
					},
					{
						middle.x(),
						1
					}
				}
			),
			make_quad_vao(r,
				{
					middle,
					{1, 1}
				}
			)
		}
	}}
// clang-format on
{}

void rectangle::update_rounded_corners_texture()
{
	if (is_all_undefined(this->params.corner_radii)) {
		this->rounded_corners_tex.reset();
		return;
	}

	sides<real> radii;
	std::transform(
		this->params.corner_radii.begin(),
		this->params.corner_radii.end(),
		radii.begin(),
		[this](const auto& r) -> real {
			if (r.get().is_undefined()) {
				return 0;
			}
			return r.get().get(this->context);
		} //
	);

	// TODO: develop algorithm to go through cache from time to time and drop zombie textures
	if (auto i = this->cache.find(radii); i != this->cache.end()) {
		if (auto t = i->second.lock()) {
			this->rounded_corners_tex = std::move(t);
			return;
		} else {
			this->cache.erase(i);
		}
	}

	auto raster_image = make_rounded_corners_texture_image(radii);

	// TODO: convert to greyscale image

	auto tex = this->context.get().ren().ctx().make_texture_2d(std::move(raster_image), {});

	using std::max;
	auto middle_px = vec2(
		max(radii[0], radii[3]),
		max(radii[0], radii[1]) //
	);

	this->rounded_corners_tex = std::make_shared<rounded_corners_texture>(
		this->context.get().ren(), //
		std::move(tex),
		middle_px
	);

	// add to cache
	this->cache.insert(std::make_pair(
		radii, //
		utki::make_weak(this->rounded_corners_tex)
	));
}
