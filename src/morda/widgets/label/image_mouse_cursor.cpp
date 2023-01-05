/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "../../util/util.hpp"

using namespace morda;

image_mouse_cursor::image_mouse_cursor(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		pile(this->context, desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "cursor"){
			this->set_cursor(this->context->loader.load<res::cursor>(get_property_value(p).to_string()));
		}
	}
}

void image_mouse_cursor::set_cursor(const utki::shared_ref<const res::cursor>& cursor) {
	this->cursor = cursor.to_shared_ptr();
	ASSERT(this->cursor)
	this->quad_tex = this->cursor->image().get();
}

bool image_mouse_cursor::on_mouse_move(const mouse_move_event& e){
	if(e.pointer_id == 0){
		this->cursorPos = e.pos;
	}
	return this->pile::on_mouse_move(e);
}

void image_mouse_cursor::render(const morda::matrix4& matrix)const{
	this->pile::render(matrix);

	if(!this->cursor){
		return;
	}
	if(!this->is_hovered(0)){
		return;
	}
	
	ASSERT(this->quad_tex)
	
	matrix4 matr(matrix);
	matr.translate(this->cursorPos);
	matr.translate(-this->cursor->hotspot());
	matr.scale(this->quad_tex->dims);
	
//	TRACE(<< "image_mouse_cursor::render(): this->cursorPos = " << this->cursorPos << " this->quadTex->dim() = " << this->quadTex->dim() << std::endl)
	
	this->quad_tex->render(matr, *this->context->renderer->pos_tex_quad_01_vao);
}
