#include "FrameContainer.hpp"

#include "../util/Layout.hpp"
#include "../util/LayoutDim.hpp"
#include "../util/Gravity.hpp"
#include "../util/util.hpp"
#include "../util/LeftBottomRightTop.hpp"

using namespace morda;


FrameContainer::FrameContainer(const stob::Node* desc) :
		Widget(desc),
		Container(desc),
		GravitatingWidget(desc)
{}



void FrameContainer::OnResize() {
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		LeftBottomRightTop margins;
		if(auto n = (*i)->GetLayoutProperty(Layout::D_Margins())){
			margins = LeftBottomRightTop::FromSTOB(n);
		}else{
			margins = LeftBottomRightTop::Default();
		}
		
		Gravity gravity;
		if(auto c = (*i)->GetLayoutProperty(Gravity::D_Gravity())){
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
		
		if(auto c = (*i)->GetLayoutProperty(LayoutDim::D_Dim())){
			dim = LayoutDim::FromSTOB(c);
		}else{
			dim = LayoutDim::Default();
		}
		
		Vec2r d = dim.ForWidget(*(*i));
		for(unsigned i = 0; i != 2; ++i){
			if(!fill[i]){
				continue;
			}
			d[i] = this->Rect().d[i] - margins.lb[i] - margins.rt[i];
		}
		
		(*i)->Resize((*i)->Measure(d));
		
		(*i)->MoveTo(gravity.PosForRect(this->Rect().d, (*i)->Rect().d, margins));
	}
}



morda::Vec2r FrameContainer::ComputeMinDim()const{
	morda::Vec2r minDim(0);
	
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		LeftBottomRightTop margins;
		if(auto n = (*i)->GetLayoutProperty(Layout::D_Margins())){
			margins = LeftBottomRightTop::FromSTOB(n);
		}else{
			margins = LeftBottomRightTop::Default();
		}
		
		morda::Vec2r dim;
		if(auto c = (*i)->GetLayoutProperty(LayoutDim::D_Dim())){
			LayoutDim ld = LayoutDim::FromSTOB(c);
			dim = (*i)->Measure(ld.ForWidget(*(*i)));
		}else{
			dim = (*i)->GetMinDim();
		}
		
		dim += margins.lb + margins.rt;
		
		ting::util::ClampBottom(minDim.x, dim.x);
		ting::util::ClampBottom(minDim.y, dim.y);
	}
	
	return minDim;
}

