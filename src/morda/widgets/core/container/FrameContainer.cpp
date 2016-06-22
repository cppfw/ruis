#include "FrameContainer.hpp"

#include "../../../util/util.hpp"

using namespace morda;


FrameContainer::FrameContainer(const stob::Node* chain) :
		Widget(chain),
		Container(chain)
{}



void FrameContainer::layOut() {
//	TRACE(<< "FrameContainer::layOut(): invoked" << std::endl)
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->getLayoutParamsDuringLayoutAs<Container::LayoutParams>(**i);
		
		(*i)->resize(this->dimForWidget(**i, lp));
		
		(*i)->moveTo(((this->rect().d - (*i)->rect().d) / 2).round());
	}
}



morda::Vec2r FrameContainer::measure(const morda::Vec2r& quotum)const{
	Vec2r ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		utki::clampBottom(ret[i], real(0));
	}
	
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->getLayoutParamsDuringLayoutAs<Container::LayoutParams>(**i);
		
		morda::Vec2r d;
		
		for(unsigned j = 0; j != d.size(); ++j){
			if(lp.dim[j] == LayoutParams::D_Max){
				if(quotum[j] >= 0){
					d[j] = quotum[j];
				}else{
					d[j] = -1;
				}
			}else if(lp.dim[j] == LayoutParams::D_Min){
				d[j] = -1;
			}else if(lp.dim[j] == LayoutParams::D_Fill){
				if(quotum[j] >= 0){
					d[j] = quotum[j];
				}else{
					d[j] = 0;
				}
			}else{
				d[j] = lp.dim[j];
			}
		}
		
		d = (*i)->measure(d);
		
		for(unsigned j = 0; j != d.size(); ++j){
			if(quotum[j] < 0){
				utki::clampBottom(ret[j], d[j]);
			}
		}
	}
	
	return ret;
}

