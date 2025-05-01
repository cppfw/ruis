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

#include <utki/types.hpp>
#include <utki/util.hpp>

#include "../../context.hpp"
#include "../../layout/linear_layout.hpp"
#include "../../layout/pile_layout.hpp"
#include "../../util/util.hpp"

using namespace std::string_literals;

using namespace ruis;

namespace {
namespace m {
using namespace ruis::make;
} // namespace m
} // namespace

nine_patch::nine_patch( //
	utki::shared_ref<ruis::context> context,
	all_parameters params,
	widget_list children
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	blending_widget( //
		this->context,
		std::move(params.blending_params)
	),
	padding(
		this->context, //
		// clang-format off
		{
			.container_params = std::move(params.container_params),
			.padding_params = std::move(params.padding_params)
		},
		// clang-format on
		std::move(children)
	),
	params(std::move(params.nine_patch_params))
{
	this->update_cur_nine_patch();
}

void nine_patch::set_nine_patch(std::shared_ptr<const res::nine_patch> np)
{
	this->params.nine_patch = std::move(np);

	if (!this->is_enabled() && this->params.disabled_nine_patch) {
		return;
	}

	this->update_cur_nine_patch();
}

void nine_patch::set_disabled_nine_patch(std::shared_ptr<const res::nine_patch> np)
{
	this->params.disabled_nine_patch = std::move(np);

	if (this->is_enabled()) {
		return;
	}

	this->update_cur_nine_patch();
}

sides<real> nine_patch::get_min_borders() const noexcept
{
	if (!this->cur_nine_patch) {
		return {0};
	}

	return this->cur_nine_patch->get_borders(this->context.get().units);
}

void nine_patch::update_cur_nine_patch()
{
	this->cur_nine_patch = this->params.nine_patch;

	if (!this->is_enabled() && this->params.disabled_nine_patch) {
		this->cur_nine_patch = this->params.disabled_nine_patch;
	}

	if (!this->cur_nine_patch) {
		this->image_texture.reset();
		return;
	}

	ASSERT(this->cur_nine_patch)
	this->image_texture = this->cur_nine_patch->image.get().get(this->context.get().units);

	this->clear_cache();
}

void nine_patch::on_enabled_change()
{
	this->padding::on_enabled_change();

	this->update_cur_nine_patch();
}

void nine_patch::render(const mat4& matrix) const
{
	this->set_blending_to_renderer();

	this->render_nine_patch(matrix);
	this->padding::render(matrix);
}

void nine_patch::render_nine_patch(const mat4& matrix) const
{
	if (!this->cur_nine_patch) {
		return;
	}

	// if there is current nine patch, there should be an image texture
	ASSERT(this->image_texture)

	auto& r = this->context.get().renderer.get();

	const auto& np = *this->cur_nine_patch;

	// left-top
	{
		ruis::mat4 matr(matrix);
		matr.scale(this->content().rect().p);

		r.render(
			matr, //
			*this->image_texture,
			np.vaos[0][0]
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

		r.render(
			matr, //
			*this->image_texture,
			np.vaos[0][1]
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

		r.render(
			matr, //
			*this->image_texture,
			np.vaos[0][2]
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

		r.render(
			matr, //
			*this->image_texture,
			np.vaos[1][0]
		);
	}

	// center
	{
		ruis::mat4 matr(matrix);
		matr.translate(this->content().rect().p);
		matr.scale(this->content().rect().d);

		r.render(
			matr, //
			*this->image_texture,
			np.vaos[1][1]
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

		r.render(
			matr, //
			*this->image_texture,
			np.vaos[1][2]
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

		r.render(
			matr, //
			*this->image_texture,
			np.vaos[2][0]
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

		r.render(
			matr, //
			*this->image_texture,
			np.vaos[2][1]
		);
	}

	// right-bottom
	{
		ruis::mat4 matr(matrix);

		auto content_x2_y2 = this->content().rect().x2_y2();

		matr.translate(content_x2_y2);
		matr.scale(this->rect().d - content_x2_y2);

		r.render(
			matr, //
			*this->image_texture,
			np.vaos[2][2]
		);
	}
}
