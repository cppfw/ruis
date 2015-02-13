#include "FrameContainer.hpp"

#include "../../util/util.hpp"

using namespace morda;


FrameContainer::FrameContainer(const stob::Node* chain) :
		Widget(chain),
		DimContainer(chain)
{}



void FrameContainer::OnResize() {
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<DimContainer::LayoutParams>(**i);
		
		Vec2r d = lp.DimForWidget(**i);
		for(unsigned i = 0; i != 2; ++i){
			if(!lp.fill[i]){
				continue;
			}
			d[i] = this->Rect().d[i];
		}
		
		(*i)->Resize(d);
		
		(*i)->MoveTo(((this->Rect().d - (*i)->Rect().d) / 2).Round());
	}
}



morda::Vec2r FrameContainer::onMeasure(const morda::Vec2r& quotum)const{
	morda::Vec2r minDim(0);
	
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<DimContainer::LayoutParams>(**i);
		
		morda::Vec2r d = lp.DimForWidget(**i);
		
		ting::util::ClampBottom(minDim.x, d.x);
		ting::util::ClampBottom(minDim.y, d.y);
	}
	
	return minDim;
}

