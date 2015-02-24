#include "FrameContainer.hpp"

#include "../../util/util.hpp"

using namespace morda;


FrameContainer::FrameContainer(const stob::Node* chain) :
		Widget(chain),
		Container(chain)
{}



void FrameContainer::OnResize() {
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<Container::LayoutParams>(**i);
		
		Vec2r d;
		for(unsigned j = 0; j != 2; ++j){
			if(lp.fill[j]){
				d[j] = this->Rect().d[j];
				continue;
			}
			d[j] = lp.dim[j];
		}
		
		(*i)->Resize((*i)->measure(d));
		
		(*i)->MoveTo(((this->Rect().d - (*i)->Rect().d) / 2).Round());
	}
}



morda::Vec2r FrameContainer::onMeasure(const morda::Vec2r& quotum)const{
	std::remove_const<std::remove_reference<decltype(quotum)>::type>::type ret(quotum);
	for(unsigned i = 0; i != ret.size(); ++i){
		ting::util::ClampBottom(ret[i], real(0));
	}
	
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<Container::LayoutParams>(**i);
		
		morda::Vec2r d;
		
		for(unsigned j = 0; j != d.size(); ++j){
			if(lp.fill[j] && quotum[j] >= 0){
				d[j] = quotum[j];
				continue;
			}
			
			d[j] = lp.dim[j];
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

