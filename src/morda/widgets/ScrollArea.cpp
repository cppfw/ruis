#include "ScrollArea.hpp"
#include "../App.hpp"


using namespace morda;



ScrollArea::ScrollArea(const stob::Node* chain) :
		Widget(chain)
{
	if(chain){
		if(auto n = chain->ThisOrNextNonProperty().node()){
			this->Add(morda::App::Inst().inflater.Inflate(*n));
		}
	}
}



void ScrollArea::OnResize() {
	//TODO:
}



bool ScrollArea::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID) {
	//TODO:
	
	return this->Container::OnMouseButton(isDown, pos - this->scrollPos, button, pointerID);
}



bool ScrollArea::OnMouseMove(const morda::Vec2r& pos, unsigned pointerID) {
	return this->Container::OnMouseMove(pos - this->scrollPos, pointerID);
}



void ScrollArea::Render(const morda::Matr4r& matrix) const {
	Matr4r matr(matrix);
	matr.Translate(-this->scrollPos);
	
	this->Container::Render(matr);
}



void ScrollArea::SetScrollPos(Vec2r newScrollPos) {
	if(this->Children().size() == 0){
		this->scrollPos = Vec2r(0);
		return;
	}
	
	this->scrollPos = newScrollPos.Rounded();
	
	auto effectiveDim = this->CalculateEffectiveDim();
	
	ting::util::ClampTop(this->scrollPos.x, effectiveDim.x);
	ting::util::ClampTop(this->scrollPos.y, effectiveDim.y);
}



void ScrollArea::SetScrollFactor(Vec2r factor) {
	if(
			factor.x < 0 || 1 < factor.x ||
			factor.y < 0 || 1 < factor.y
		)
	{
		throw morda::Exc("ScrollArea::SetScrollFactor(): ");
	}
	
	auto effectiveDim = this->CalculateEffectiveDim();
	
	this->SetScrollPos(effectiveDim.CompMul(factor));
}



Vec2r ScrollArea::ScrollFactor()const{
	if(this->Children().size() == 0){
		return Vec2r(0);
	}
	
	auto effectiveDim = this->CalculateEffectiveDim();
	
	return this->ScrollPos().CompDiv(effectiveDim);
}


