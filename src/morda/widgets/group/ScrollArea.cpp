#include "ScrollArea.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"


using namespace morda;



ScrollArea::ScrollArea(const puu::forest& desc) :
		widget(desc),
		container(desc)
{}



bool ScrollArea::on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID) {
	Vec2r d = -this->curScrollPos;
	return this->Container::on_mouse_button(isDown, pos - d, button, pointerID);
}



bool ScrollArea::on_mouse_move(const morda::Vec2r& pos, unsigned pointerID) {
	Vec2r d = -this->curScrollPos;
	return this->Container::on_mouse_move(pos - d, pointerID);
}



void ScrollArea::render(const morda::Matr4r& matrix) const {
	Vec2r d = -this->curScrollPos;

	Matr4r matr(matrix);
	matr.translate(d);

	this->Container::render(matr);
}

void ScrollArea::clampScrollPos() {
	if(this->effectiveDim.x < 0){
		this->curScrollPos.x = 0;
	}

	if(this->effectiveDim.y < 0){
		this->curScrollPos.y = 0;
	}
}



void ScrollArea::setScrollPos(const Vec2r& newScrollPos) {
	this->curScrollPos = newScrollPos.rounded();

	this->clampScrollPos();
	this->updateScrollFactor();
}



void ScrollArea::setScrollPosAsFactor(const Vec2r& factor){
	Vec2r newScrollPos = this->effectiveDim.compMul(factor);

	this->setScrollPos(newScrollPos);
}


void ScrollArea::updateScrollFactor(){
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

Vec2r ScrollArea::dimForWidget(const Widget& w, const LayoutParams& lp)const{
	Vec2r d;
	for(unsigned i = 0; i != 2; ++i){
		if(lp.dim[i] == LayoutParams::fill_c){
			d[i] = this->rect().d[i];
		}else if(lp.dim[i] == LayoutParams::min_c || lp.dim[i] == LayoutParams::max_c){
			d[i] = -1; // will be updated below
		}else{
			d[i] = lp.dim[i];
		}
	}
	if(d.x < 0 || d.y < 0){
		Vec2r md = w.measure(d);
		for(unsigned i = 0; i != md.size(); ++i){
			if(d[i] < 0){
				if(lp.dim[i] == LayoutParams::max_c && md[i] < this->rect().d[i]){
					d[i] = this->rect().d[i];
				}else{
					d[i] = md[i];
				}
			}
		}
	}
	return d;
}

void ScrollArea::arrangeWidgets() {
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->get_layout_params(**i);

		auto d = this->dimForWidget(**i, lp);

		(*i)->resize(d);
	}
}


void ScrollArea::lay_out(){
	this->arrangeWidgets();
	this->updateEffectiveDim();

	//distance of content's bottom right corner from bottom right corner of the ScrollArea
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

void ScrollArea::on_children_changed(){
	this->Container::on_children_changed();
	this->arrangeWidgets();
	this->updateEffectiveDim();
}

void ScrollArea::updateEffectiveDim(){
	morda::Vec2r minDim(0);

	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->getLayoutParamsAs<LayoutParams>(**i);

		morda::Vec2r d = this->dimForWidget(**i, lp) + (*i)->rect().p;

		utki::clampBottom(minDim.x, d.x);
		utki::clampBottom(minDim.y, d.y);
	}

	this->effectiveDim = minDim - this->rect().d;
	this->updateScrollFactor();
}

Vec2r ScrollArea::visibleAreaFraction() const noexcept{
	auto ret = this->rect().d.compDiv(this->rect().d + this->effectiveDim);

	for(unsigned i = 0; i != ret.size(); ++i){
		utki::clampTop(ret[i], real(1));
	}

	return ret;
}
