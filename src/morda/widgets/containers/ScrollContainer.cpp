#include "ScrollContainer.hpp"
#include "../../App.hpp"
#include "../../util/util.hpp"


using namespace morda;



ScrollContainer::ScrollContainer(const stob::Node* chain) :
		Widget(chain),
		DimContainer(chain)
{}



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
	Vec2r factor = this->scrollPos.CompDiv(this->effectiveDim);
	
	if(this->scrollFactor == factor){
		return;
	}
	
	for(unsigned i = 0; i != 2; ++i){
		if(this->effectiveDim[i] <= 0){
			this->scrollFactor[i] = 0;
		}else{
			this->scrollFactor[i] = this->scrollPos[i] / this->effectiveDim[i];
		}
	}
	
	if(this->onScrollFactorChanged){
		this->onScrollFactorChanged(*this);
	}
}

void ScrollContainer::OnResize(){
	this->DimContainer::OnResize();
	this->UpdateEffectiveDim();

	//distance of content's bottom right corner from bottom right corner of the ScrollContainer
	Vec2r br = this->scrollPos - this->effectiveDim;

	if(br.x > 0){
		if(br.x <= this->scrollPos.x){
			this->scrollPos.x -= br.x;
		}else{
			this->scrollPos.x = 0;
		}
	}
	
	if(br.y > 0){
		if(br.y <= this->scrollPos.y){
			this->scrollPos.y -= br.y;
		}else{
			this->scrollPos.y = 0;
		}
	}
}

void ScrollContainer::OnChildrenListChanged(){
	this->DimContainer::OnResize();
	this->UpdateEffectiveDim();
}

void ScrollContainer::UpdateEffectiveDim(){
	this->effectiveDim = this->DimContainer::Measure(Vec2r(-1)) - this->Rect().d;
	this->UpdateScrollFactor();
}
