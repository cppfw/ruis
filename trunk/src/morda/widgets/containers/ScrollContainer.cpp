#include "ScrollContainer.hpp"
#include "../../App.hpp"
#include "../../util/util.hpp"


using namespace morda;



ScrollContainer::ScrollContainer(const stob::Node* chain) :
		Widget(chain),
		DimContainer(chain)

{}

morda::Vec2r ScrollContainer::ComputeMinDim() const {
	return Vec2r(0);
}



bool ScrollContainer::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID) {
	Vec2r d = this->scrollPos;
	d.y -= this->effectiveDim.y;
	d.x = -d.x;
	return this->DimContainer::OnMouseButton(isDown, pos - d, button, pointerID);
}



bool ScrollContainer::OnMouseMove(const morda::Vec2r& pos, unsigned pointerID) {
	Vec2r d = this->scrollPos;
	d.y -= this->effectiveDim.y;
	d.x = -d.x;
	return this->DimContainer::OnMouseMove(pos - d, pointerID);
}



void ScrollContainer::Render(const morda::Matr4r& matrix) const {
	Vec2r d = this->scrollPos;
	d.y -= this->effectiveDim.y;
	d.x = -d.x;
	
	Matr4r matr(matrix);
	matr.Translate(d);
	
	this->DimContainer::Render(matr);
}

void ScrollContainer::ClampScrollPos() {
	if(this->effectiveDim.x < 0){
		this->scrollPos.x = 0;
	}
	
	if(this->effectiveDim.y < 0){
		this->scrollPos.y = 0;
	}
}



void ScrollContainer::SetScrollPos(const Vec2r& newScrollPos) {
	this->scrollPos = newScrollPos.Rounded();
	
	this->ClampScrollPos();
	this->UpdateScrollFactor();
}



void ScrollContainer::SetScrollPosAsFactor(const Vec2r& factor){	
	Vec2r newScrollPos = this->effectiveDim.CompMul(factor);
	
	this->SetScrollPos(newScrollPos);
}


void ScrollContainer::UpdateScrollFactor(){
	//at this point effective dimension should be updated			
	this->scrollFactor = this->scrollPos.CompDiv(this->effectiveDim);
}
