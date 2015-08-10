#include "FrameContainer.hpp"

#include "../../util/util.hpp"

using namespace morda;


FrameContainer::FrameContainer(const stob::Node* chain) :
		Widget(chain),
		Container(chain)
{}



void FrameContainer::layOut() {
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->getLayoutParamsAs<Container::LayoutParams>(**i);
		
		(*i)->Resize(this->dimForWidget(**i, lp));
		
		(*i)->MoveTo(((this->Rect().d - (*i)->Rect().d) / 2).Round());
	}
}



morda::Vec2r FrameContainer::onMeasure(const morda::Vec2r& quotum)const{
	Vec2r ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		ting::util::ClampBottom(ret[i], real(0));
	}
	
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->getLayoutParamsAs<Container::LayoutParams>(**i);
		
		morda::Vec2r d;
		
		for(unsigned j = 0; j != d.size(); ++j){
			if(lp.dim[j] == LayoutParams::D_Max){
				if(quotum[j] >= 0){
					d[j] = quotum[j];
				}else{
					d[j] = -1;
				}
			}else if(lp.dim[j] == LayoutParams::D_Min || lp.dim[j] < 0){
				d[j] = -1;
			}else{
				d[j] = lp.dim[j];
			}
		}
		
		d = (*i)->measure(d);
		
		for(unsigned j = 0; j != d.size(); ++j){
			if(quotum[j] < 0){
				ting::util::ClampBottom(ret[j], d[j]);
			}
		}
	}
	
	return ret;
}

