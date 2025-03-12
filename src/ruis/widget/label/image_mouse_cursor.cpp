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

#include "image_mouse_cursor.hpp"

#include "../../context.hpp"
#include "../../layout/pile_layout.hpp"
#include "../../util/util.hpp"

using namespace ruis;

image_mouse_cursor::image_mouse_cursor(
	utki::shared_ref<ruis::context> context, //
	all_parameters params
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	params(std::move(params.mouse_cursor_params))
{}

void image_mouse_cursor::set_cursor(const utki::shared_ref<const res::cursor>& cursor)
{
	this->params.cursor = cursor.to_shared_ptr();
	ASSERT(this->params.cursor)
	this->quad_tex.reset();
}

bool image_mouse_cursor::on_mouse_move(const mouse_move_event& e)
{
	if (e.pointer_id == 0) {
		this->cursor_pos = e.pos;
	}
	return this->widget::on_mouse_move(e);
}

void image_mouse_cursor::render(const ruis::matrix4& matrix) const
{
	this->widget::render(matrix);

	if (!this->params.cursor) {
		return;
	}

	if (!this->is_hovered(0)) {
		return;
	}

	if (!this->quad_tex) {
		ASSERT(this->params.cursor)
		this->quad_tex = this->params.cursor->image().get(this->context.get().units).to_shared_ptr();
	}

	ASSERT(this->quad_tex)

	matrix4 matr(matrix);
	matr.translate(this->cursor_pos);
	matr.translate(-this->params.cursor->hotspot());
	matr.scale(this->quad_tex->dims().to<real>());

	//	TRACE(<< "image_mouse_cursor::render(): this->cursorPos = " << this->cursorPos << " this->quadTex->dim() = " <<
	// this->quadTex->dim() << std::endl)

	this->quad_tex->render(matr, this->context.get().renderer.get().obj().pos_tex_quad_01_vao.get());
}
