#include "MinSizeLayout.hpp"
#include "../Container.hpp"



using namespace morda;



//override
void MinSizeLayout::ArrangeWidgets(Container& cont)const{
	for(const ting::Ref<Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
		(*c)->Resize((*c)->GetMinDim());
	}
}



//override
morda::Vec2f MinSizeLayout::ComputeMinDim(const Container& cont)const throw(){
	morda::Vec2f minDim(0);
	for(const ting::Ref<const Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
		morda::Vec2f tr = (*c)->Rect().p + (*c)->GetMinDim();
		if(tr.x > minDim.x){
			minDim.x = tr.x;
		}
		if(tr.y > minDim.y){
			minDim.y = tr.y;
		}
	}
	return minDim;
}
