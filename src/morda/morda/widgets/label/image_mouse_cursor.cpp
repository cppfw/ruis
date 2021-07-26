/*
morda GUI framework
Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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
			this->setCursor(this->context->loader.load<res::cursor>(get_property_value(p).to_string()));
		}
	}
}

void image_mouse_cursor::setCursor(std::shared_ptr<const res::cursor> cursor) {
	this->cursor = std::move(cursor);
	this->quadTex.reset();
	if(this->cursor){
		this->quadTex = this->cursor->image().get();
	}
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
	
	ASSERT(this->quadTex)
	
	matrix4 matr(matrix);
	matr.translate(this->cursorPos);
	matr.translate(-this->cursor->hotspot());
	matr.scale(this->quadTex->dims);
	
//	TRACE(<< "image_mouse_cursor::render(): this->cursorPos = " << this->cursorPos << " this->quadTex->dim() = " << this->quadTex->dim() << std::endl)
	
	this->quadTex->render(matr, *this->context->renderer->pos_tex_quad_01_vao);
}
