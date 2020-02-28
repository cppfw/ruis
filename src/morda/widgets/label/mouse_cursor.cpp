#include "mouse_cursor.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

using namespace morda;

mouse_cursor::mouse_cursor(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "cursor"){
			this->setCursor(this->context->loader.load<res_cursor>(get_property_value(p).to_string()));
		}
	}
}

void mouse_cursor::setCursor(std::shared_ptr<const res_cursor> cursor) {
	this->cursor = std::move(cursor);
	this->quadTex.reset();
	if(this->cursor){
		this->quadTex = this->cursor->image().get();
	}
}

bool mouse_cursor::on_mouse_move(const morda::Vec2r& pos, unsigned pointerID) {
	if(pointerID == 0){
		this->cursorPos = pos;
	}
	return false;
}

void mouse_cursor::render(const morda::matrix4& matrix) const {
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
	
//	TRACE(<< "mouse_cursor::render(): this->cursorPos = " << this->cursorPos << " this->quadTex->dim() = " << this->quadTex->dim() << std::endl)
	
	//TODO:
//	s.setMatrix(matr);
	this->quadTex->render(matr, *this->context->renderer->pos_tex_quad_01_vao);
}
