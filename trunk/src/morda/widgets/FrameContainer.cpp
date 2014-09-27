#include "FrameContainer.hpp"

#include "../util/Layout.hpp"
#include "../util/LayoutDim.hpp"
#include "../util/Gravity.hpp"
#include "../util/util.hpp"

using namespace morda;


//TODO: add edge margins


FrameContainer::FrameContainer(const stob::Node* desc) :
		Widget(desc),
		Container(desc),
		GravitatingWidget(desc)
{}



void FrameContainer::OnResize() {
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		Gravity gravity;
		
		if(const stob::Node* c = (*i)->GetLayoutProperty(Gravity::D_Gravity())){
			gravity = Gravity::FromSTOB(c);
		}else{
			gravity = this->gravity();
		}
		
		Vec2b fill;
		
		if(auto n = (*i)->GetLayoutProperty(Layout::D_Fill())){
			fill = Vec2bFromSTOB(n);
		}else{
			fill = decltype(fill)(false, false);
		}
		
		LayoutDim dim;
		
		if(const stob::Node* c = (*i)->GetLayoutProperty(LayoutDim::D_Dim())){
			dim = LayoutDim::FromSTOB(c);
		}else{
			dim = LayoutDim::Default();
		}
		
		Vec2r d = dim.ForWidget(*(*i));
		for(unsigned i = 0; i != 2; ++i){
			if(!fill[i]){
				continue;
			}
			ting::util::ClampBottom(d[i], this->Rect().d[i]);
		}
		
		(*i)->Resize((*i)->Measure(d));
		
		(*i)->MoveTo(gravity.PosForRect(this->Rect().d, (*i)->Rect().d));
	}
}



morda::Vec2r FrameContainer::ComputeMinDim()const{
	morda::Vec2r minDim(0);
	
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		morda::Vec2r dim;
		if(const stob::Node* c = (*i)->GetLayoutProperty(LayoutDim::D_Dim())){
			LayoutDim ld = LayoutDim::FromSTOB(c);
			dim = (*i)->Measure(ld.ForWidget(*(*i)));
		}else{
			dim = (*i)->GetMinDim();
		}
		
		if(dim.x > minDim.x){
			minDim.x = dim.x;
		}
		if(dim.y > minDim.y){
			minDim.y = dim.y;
		}
	}
	
	return minDim;
}

