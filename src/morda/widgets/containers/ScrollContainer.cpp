#include "ScrollContainer.hpp"
#include "../../App.hpp"


using namespace morda;



ScrollContainer::ScrollContainer(const stob::Node* chain) :
		Widget(chain),
		DimContainer(chain)

{}

morda::Vec2r ScrollContainer::ComputeMinDim() const {
	return Vec2r(0);
}



bool ScrollContainer::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID) {
	//TODO:
	
	return this->DimContainer::OnMouseButton(isDown, pos - this->scrollPos, button, pointerID);
}



bool ScrollContainer::OnMouseMove(const morda::Vec2r& pos, unsigned pointerID) {
	//TODO:
	
	return this->DimContainer::OnMouseMove(pos - this->scrollPos, pointerID);
}



void ScrollContainer::Render(const morda::Matr4r& matrix) const {
	Matr4r matr(matrix);
	matr.Translate(-this->scrollPos);
	
	this->DimContainer::Render(matr);
}



void ScrollContainer::SetScrollPos(Vec2r newScrollPos) {
	if(this->Children().size() == 0){
		this->scrollPos = Vec2r(0);
		return;
	}
	
	this->scrollPos = newScrollPos.Rounded();
}



void ScrollContainer::SetScrollFactor(Vec2r factor) {
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



Vec2r ScrollContainer::ScrollFactor()const{
	if(this->Children().size() == 0){
		return Vec2r(0);
	}
	
	auto effectiveDim = this->CalculateEffectiveDim();
	
	auto factor = this->ScrollPos().CompDiv(effectiveDim);
	
	ting::util::ClampTop(factor.x, decltype(factor)::T_Component(1));
	ting::util::ClampTop(factor.y, decltype(factor)::T_Component(1));
	
	return factor;
}


