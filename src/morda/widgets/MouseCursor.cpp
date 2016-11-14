#include "MouseCursor.hpp"

#include "../Morda.hpp"

#include "../util/util.hpp"


using namespace morda;


MouseCursor::MouseCursor(const stob::Node* chain) :
		Widget(chain)
{
	if(auto n = getProperty(chain, "cursor")){
		this->setCursor(morda::Morda::inst().resMan.load<ResCursor>(n->value()));
	}
}

void MouseCursor::setCursor(std::shared_ptr<const ResCursor> cursor) {
	this->cursor = std::move(cursor);
	this->quadTex.reset();
	if(this->cursor){
		this->quadTex = this->cursor->image().get();
	}
}

bool MouseCursor::onMouseMove(const morda::Vec2r& pos, unsigned pointerID) {
	if(pointerID == 0){
		this->cursorPos = pos;
	}
	return false;
}

void MouseCursor::render(const morda::Matr4r& matrix) const {
	if(!this->cursor){
		return;
	}
	if(!this->isHovered(0)){
		return;
	}
	
	ASSERT(this->quadTex)
	
	auto& s = morda::Morda::inst().shaders.posTexShader;
	
	Matr4r matr(matrix);
	matr.translate(this->cursorPos);
	matr.translate(-this->cursor->hotspot());
	matr.scale(this->quadTex->dim());
	
//	TRACE(<< "MouseCursor::render(): this->cursorPos = " << this->cursorPos << " this->quadTex->dim() = " << this->quadTex->dim() << std::endl)
	
	s.setMatrix(matr);
	this->quadTex->render(matr, s);
}

