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
		ASSERT(this->nine_patch_tex)
		const auto& t = *this->nine_patch_tex;

		// left-top
		{
			ruis::mat4 matr(matrix);
			matr.scale(t.borders_px.left_top());

			r.shaders().pos_tex->render(
				matr, //
				t.vaos[0][0],
				t.tex
			);
		}

		// top
		{
			ruis::mat4 matr(matrix);
			matr.translate(
				t.borders_px.left(), //
				0
			);
			matr.scale(
				this->content().rect().d.x(), //
				t.borders_px.top()
			);

			r.shaders().color_pos->render(
				matr, //
				r.obj().pos_quad_01_vao.get(),
				this->get_current_color().to_vec4f()
			);
		}

		// right-top
		{
			ruis::mat4 matr(matrix);
			matr.translate(
				this->rect().d.x() - t.borders_px.right(), //
				0
			);
			matr.scale(t.borders_px.right_top());

			r.shaders().pos_tex->render(
				matr, //
				t.vaos[0][1],
				t.tex
			);
		}

		// TODO:
	}

	this->margins::render(matrix);
}

namespace {
auto fill_rounded_corners_rect(const sides<real>& borders)
{
	veg::canvas canvas(borders.dims().to<uint32_t>());

	// TODO:
	canvas.rectangle(
		{
			{0, 0},
			canvas.dims.to<veg::real>()
    }, //
		{borders.left(), borders.top()}
	);

	canvas.set_source({1, 0, 0, 1}); // TODO: white
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

	// clang-format off
	return ren.ctx().create_vertex_array(
		{
			ren.obj().quad_01_vbo,
			ren.ctx().create_vertex_buffer(tex_coords_array)
		},
		ren.obj().quad_fan_indices,
		ruis::render::vertex_array::mode::triangle_fan
	);
	// clang-format on
}

} // namespace

rectangle::nine_patch_texture::nine_patch_texture(
	ruis::render::renderer& r, //
	utki::shared_ref<const render::texture_2d> tex,
	const sides<real>& borders_px
) :
	nine_patch_texture(
		r, //
		std::move(tex),
		borders_px,
		borders_px.left_top().comp_div(tex.get().dims().to<real>())
	)
{}

rectangle::nine_patch_texture::nine_patch_texture(
	ruis::render::renderer& r, //
	utki::shared_ref<const render::texture_2d> tex,
	const sides<real>& borders_px,
	vec2 middle
) :
	borders_px(borders_px),
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
					{
						1,
						1
					}
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

	auto raster_image = fill_rounded_corners_rect(borders);

	// TODO: convert to greyscale image

	this->nine_patch_tex = std::make_shared<nine_patch_texture>(
		this->context.get().ren(), //
		this->context.get().ren().ctx().create_texture_2d(std::move(raster_image), {}),
		borders
	);
}
