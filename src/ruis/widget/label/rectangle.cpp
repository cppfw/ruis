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
	margins(//
		this->context,
		{
			.container_params = std::move(params.container_params),
			.frame_params = std::move(params.frame_params)
		},
		std::move(children)
	),
	color_widget( //
		this->context,
		std::move(params.color_params)
	),
	params(std::move(params.rect_params))
{
	this->update_nine_patch_text();
}

void rectangle::render(const ruis::matrix4& matrix) const
{
	auto& r = this->context.get().renderer.get();

	r.render_context.get().set_simple_alpha_blending();

	if (!this->nine_patch_tex) {
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

	this->margins::render(matrix);
}

void rectangle::render_rounder_corners(const mat4& matrix) const
{
	auto& r = this->context.get().renderer.get();

	color clr = this->get_current_color();

	ASSERT(this->nine_patch_tex)
	const auto& t = *this->nine_patch_tex;

	// left-top
	{
		ruis::mat4 matr(matrix);
		matr.scale(this->content().rect().p);

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
			this->content().rect().p.x(), //
			0
		);
		matr.scale(
			this->content().rect().d.x(), //
			this->content().rect().p.y()
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
			this->content().rect().x2(), //
			0
		);
		matr.scale(
			this->rect().d.x() - this->content().rect().x2(), //
			this->content().rect().p.y()
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
			this->content().rect().p.y()
		);
		matr.scale(
			this->content().rect().p.x(), //
			this->content().rect().d.y()
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
		matr.translate(this->content().rect().p);
		matr.scale(this->content().rect().d);

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
			this->content().rect().x2(), //
			this->content().rect().p.y()
		);
		matr.scale(
			this->rect().d.x() - this->content().rect().x2(), //
			this->content().rect().d.y()
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
			this->content().rect().y2()
		);
		matr.scale(
			this->content().rect().p.x(), //
			this->rect().d.y() - this->content().rect().y2()
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
			this->content().rect().p.x(), //
			this->content().rect().y2()
		);
		matr.scale(
			this->content().rect().d.x(), //
			this->rect().d.y() - this->content().rect().y2()
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

		auto content_x2_y2 = this->content().rect().x2_y2();

		matr.translate(content_x2_y2);
		matr.scale(this->rect().d - content_x2_y2);

		r.shaders().color_pos_tex->render(
			matr, //
			t.vaos[1][1],
			clr,
			t.tex
		);
	}
}

namespace {
auto make_rounded_corners_texture_image(const sides<real>& borders)
{
	// approximate 90 degree arc with bezier curve which matches the arc at 45 degree point
	// and has the same tangent as an arc at 45 degree point
	using std::sqrt;
	constexpr const real arc_bezier_param = real(4 * (sqrt(2) - 1) / 3);

	veg::canvas canvas(borders.dims().to<uint32_t>());

	canvas.move_abs({0, borders.top()});

	canvas.cubic_curve_rel(
		{0, -arc_bezier_param * borders.top()},
		{borders.left() * (1 - arc_bezier_param), -borders.top()},
		{borders.left(), -borders.top()}
	);

	canvas.cubic_curve_rel(
		{arc_bezier_param * borders.right(), 0},
		{borders.right(), borders.top() * (1 - arc_bezier_param)},
		{borders.right(), borders.top()}
	);

	canvas.cubic_curve_rel(
		{0, arc_bezier_param * borders.bottom()},
		{-borders.right() * (1 - arc_bezier_param), borders.bottom()},
		{-borders.right(), borders.bottom()}
	);

	canvas.cubic_curve_rel(
		{-arc_bezier_param * borders.left(), 0},
		{-borders.left(), -borders.bottom() * (1 - arc_bezier_param)},
		{-borders.left(), -borders.bottom()}
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

	return ren.ctx().create_vertex_array(
		// clang-format off
		{
			ren.obj().quad_01_vbo,
			ren.ctx().create_vertex_buffer(tex_coords_array)
		},
		// clang-format on
		ren.obj().quad_fan_indices,
		ruis::render::vertex_array::mode::triangle_fan
	);
}

} // namespace

rectangle::nine_patch_texture::nine_patch_texture(
	ruis::render::renderer& r, //
	utki::shared_ref<const render::texture_2d> tex,
	vec2 middle
) :
	tex(std::move(tex)),
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

void rectangle::update_nine_patch_text()
{
	if (!this->params.rounded_corners) {
		this->nine_patch_tex.reset();
		return;
	}

	auto borders = this->get_actual_borders();

	auto raster_image = make_rounded_corners_texture_image(borders);

	// TODO: convert to greyscale image

	auto tex = this->context.get().ren().ctx().create_texture_2d(std::move(raster_image), {});

	auto middle = borders.left_top().comp_div(tex.get().dims().to<real>());

	this->nine_patch_tex = std::make_shared<nine_patch_texture>(
		this->context.get().ren(), //
		std::move(tex),
		middle
	);
}
