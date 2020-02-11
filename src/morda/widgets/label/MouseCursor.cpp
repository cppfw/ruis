#include "MouseCursor.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"


using namespace morda;


MouseCursor::MouseCursor(const puu::forest& desc) :
		widget(desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "cursor"){
			this->setCursor(morda::gui::inst().context->loader.load<ResCursor>(get_property_value(p).to_string()));
		}
	}
}

void MouseCursor::setCursor(std::shared_ptr<const ResCursor> cursor) {
	this->cursor = std::move(cursor);
	this->quadTex.reset();
	if(this->cursor){
		this->quadTex = this->cursor->image().get();
	}
}

bool MouseCursor::on_mouse_move(const morda::Vec2r& pos, unsigned pointerID) {
	if(pointerID == 0){
		this->cursorPos = pos;
	}
	return false;
}

void MouseCursor::render(const morda::Matr4r& matrix) const {
	if(!this->cursor){
		return;
	}
	if(!this->is_hovered(0)){
		return;
	}
	
	ASSERT(this->quadTex)
	
	Matr4r matr(matrix);
	matr.translate(this->cursorPos);
	matr.translate(-this->cursor->hotspot());
	matr.scale(this->quadTex->dims());
	
//	TRACE(<< "MouseCursor::render(): this->cursorPos = " << this->cursorPos << " this->quadTex->dim() = " << this->quadTex->dim() << std::endl)
	
	//TODO:
//	s.setMatrix(matr);
	this->quadTex->render(matr, *morda::inst().context->renderer->posTexQuad01VAO);
}

