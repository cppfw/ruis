#include "ScrollContainer.hpp"
#include "../../App.hpp"
#include "../../util/util.hpp"


using namespace morda;



ScrollContainer::ScrollContainer(const stob::Node* chain) :
		Widget(chain),
		Container(chain)
{}



bool ScrollContainer::onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID) {
	Vec2r d = this->curScrollPos;
	d.y -= this->effectiveDim.y;
	d.x = -d.x;
	return this->Container::onMouseButton(isDown, pos - d, button, pointerID);
}



bool ScrollContainer::onMouseMove(const morda::Vec2r& pos, unsigned pointerID) {
	Vec2r d = this->curScrollPos;
	d.y -= this->effectiveDim.y;
	d.x = -d.x;
	return this->Container::onMouseMove(pos - d, pointerID);
}



void ScrollContainer::render(const morda::Matr4r& matrix) const {
	Vec2r d = this->curScrollPos;
	d.y -= this->effectiveDim.y;
	d.x = -d.x;
	
	Matr4r matr(matrix);
	matr.translate(d);
	
	this->Container::render(matr);
}

void ScrollContainer::ClampScrollPos() {
	if(this->effectiveDim.x < 0){
		this->curScrollPos.x = 0;
	}
	
	if(this->effectiveDim.y < 0){
		this->curScrollPos.y = 0;
	}
}



void ScrollContainer::setScrollPos(const Vec2r& newScrollPos) {
	this->curScrollPos = newScrollPos.rounded();
	
	this->ClampScrollPos();
	this->UpdateScrollFactor();
}



void ScrollContainer::setScrollPosAsFactor(const Vec2r& factor){	
	Vec2r newScrollPos = this->effectiveDim.compMul(factor);
	
	this->setScrollPos(newScrollPos);
}


void ScrollContainer::UpdateScrollFactor(){
	//at this point effective dimension should be updated
	Vec2r factor = this->curScrollPos.compDiv(this->effectiveDim);
	
	if(this->curScrollFactor == factor){
		return;
	}
	
	for(unsigned i = 0; i != 2; ++i){
		if(this->effectiveDim[i] <= 0){
			this->curScrollFactor[i] = 0;
		}else{
			this->curScrollFactor[i] = this->curScrollPos[i] / this->effectiveDim[i];
		}
	}
}

void ScrollContainer::arrangeWidgets() {
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->getLayoutParamsAs<LayoutParams>(**i);
		
		auto d = this->dimForWidget(**i, lp);
		
		(*i)->resize(d);
	}
}


void ScrollContainer::layOut(){
	this->arrangeWidgets();
	this->UpdateEffectiveDim();

	//distance of content's bottom right corner from bottom right corner of the ScrollContainer
	Vec2r br = this->curScrollPos - this->effectiveDim;

	if(br.x > 0){
		if(br.x <= this->curScrollPos.x){
			this->curScrollPos.x -= br.x;
		}else{
			this->curScrollPos.x = 0;
		}
	}
	
	if(br.y > 0){
		if(br.y <= this->curScrollPos.y){
			this->curScrollPos.y -= br.y;
		}else{
			this->curScrollPos.y = 0;
		}
	}
}

void ScrollContainer::onChildrenListChanged(){
	this->Container::onChildrenListChanged();
	this->arrangeWidgets();
	this->UpdateEffectiveDim();
}

void ScrollContainer::UpdateEffectiveDim(){
	morda::Vec2r minDim(0);

	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->getLayoutParamsAs<LayoutParams>(**i);

		morda::Vec2r d = this->dimForWidget(**i, lp) + (*i)->rect().p;
		
		utki::clampBottom(minDim.x, d.x);
		utki::clampBottom(minDim.y, d.y);
	}	
	
	this->effectiveDim = minDim - this->rect().d;
	this->UpdateScrollFactor();
}
