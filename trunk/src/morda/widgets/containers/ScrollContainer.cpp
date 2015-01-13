#include "ScrollContainer.hpp"
#include "../../App.hpp"
#include "../../util/util.hpp"


using namespace morda;



ScrollContainer::ScrollContainer(const stob::Node* chain) :
		Widget(chain),
		DimContainer(chain)

{
	if(auto n = GetProperty(chain, "reverseX")){
		this->reverse.x = n->AsBool();
	}
	
	if(auto n = GetProperty(chain, "reverseY")){
		this->reverse.y = n->AsBool();
	}
}

morda::Vec2r ScrollContainer::ComputeMinDim() const {
	return Vec2r(0);
}



bool ScrollContainer::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID) {
	return this->DimContainer::OnMouseButton(isDown, pos + this->scrollPos, button, pointerID);
}



bool ScrollContainer::OnMouseMove(const morda::Vec2r& pos, unsigned pointerID) {
	return this->DimContainer::OnMouseMove(pos + this->scrollPos, pointerID);
}



void ScrollContainer::Render(const morda::Matr4r& matrix) const {
	Matr4r matr(matrix);
	matr.Translate(-this->scrollPos);
	
	this->DimContainer::Render(matr);
}

void ScrollContainer::ClampScrollPos(const Vec2r& effectiveDim) {
	if(effectiveDim.x < 0){
		this->scrollPos.x = 0;
	}
	
	if(effectiveDim.y < 0){
		this->scrollPos.y = effectiveDim.y;
	}
}

void ScrollContainer::OnResize() {
	this->ClampScrollPos(this->ComputeEffectiveDim());
}



void ScrollContainer::SetScrollPos(const Vec2r& newScrollPos) {
	if(this->Children().size() == 0){
		this->scrollPos = Vec2r(0);
		return;
	}
	
	this->scrollPos = newScrollPos.Rounded();
	
	this->ClampScrollPos(this->ComputeEffectiveDim());
}



void ScrollContainer::SetScrollFactor(const Vec2r& factor) {
	if(
			factor.x < 0 || 1 < factor.x ||
			factor.y < 0 || 1 < factor.y
		)
	{
		throw morda::Exc("ScrollArea::SetScrollFactor(): factor is out of [0:1] range.");
	}
	
	auto effectiveDim = this->ComputeEffectiveDim();
	
	this->scrollPos = effectiveDim.CompMul(factor).Rounded();
	
	this->ClampScrollPos(effectiveDim);
}



Vec2r ScrollContainer::ScrollFactor()const{
	if(this->Children().size() == 0){
		return Vec2r(0);
	}
	
	auto effectiveDim = this->ComputeEffectiveDim();
	
	auto factor = this->ScrollPos().CompDiv(effectiveDim);
	
	ting::util::ClampTop(factor.x, decltype(factor)::T_Component(1));
	ting::util::ClampTop(factor.y, decltype(factor)::T_Component(1));
	
	return factor;
}


