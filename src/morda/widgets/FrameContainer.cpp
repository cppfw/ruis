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
		LayoutDim dim;
		Gravity gravity;
		Vec2b fill;
		
		if(const stob::Node* layout = (*i)->GetPropertyNode(Container::D_Layout())){
			dim = LayoutDim::FromSTOB(layout->Child(LayoutDim::D_Dim()).node());
			
			if(const stob::Node* g = layout->Child(Gravity::D_Gravity()).node()){
				gravity = Gravity::FromSTOB(*g);
			}else{
				gravity = this->gravity();
			}
			
			if(const stob::Node* n = layout->GetProperty(Layout::D_Fill())){
				fill = TwoBoolsFromSTOB(n);
			}else{
				fill = decltype(fill)(false, false);
			}
		}else{
			dim = LayoutDim::Default();
			gravity = this->gravity();
			fill = decltype(fill)(false, false);
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
		morda::Vec2r dim = (*i)->GetMinDim();
		if(const stob::Node* layout = (*i)->GetPropertyNode(Container::D_Layout())){
			LayoutDim ld = LayoutDim::FromLayout(*layout);

			dim = (*i)->Measure(ld.ForWidget(*(*i)));
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

