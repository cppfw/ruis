#include "FrameContainer.hpp"

#include "../util/LayoutDim.hpp"
#include "../util/Gravity.hpp"
#include "../util/util.hpp"

using namespace morda;


//TODO: add margins


FrameContainer::FrameContainer(const stob::Node& desc) :
		Widget(desc),
		Container(desc),
		PaddedWidget(desc)
{}



void FrameContainer::OnResize() {
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		LayoutDim dim;
		Gravity gravity;

		if(const stob::Node* layout = (*i)->GetPropertyNode(Container::D_Layout())){
			dim = LayoutDim::FromSTOB(layout->Child(LayoutDim::D_Dim()).node());
			gravity = Gravity::FromLayout(*layout);
		}else{
			dim = LayoutDim::Default();
			gravity = Gravity::Default();
		}
		
		(*i)->Resize((*i)->Measure(dim.ForWidget(*this, *(*i))));
		
		(*i)->MoveTo(gravity.PosForRect(*this, (*i)->Rect().d));
	}
}



morda::Vec2f FrameContainer::ComputeMinDim()const{
	morda::Vec2f minDim(0);
	
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		morda::Vec2f dim = (*i)->GetMinDim();
		if((*i)->Prop()){
			LayoutDim ld = LayoutDim::FromPropLayout(*(*i)->Prop());
			
			//FRACTION min size is 0.
			for(unsigned j = 0; j != 2; ++j){
				if(ld[j].unit == LayoutDim::FRACTION){
					ld[j].unit = LayoutDim::PIXEL;
					ld[j].value = 0;
				}
			}

			dim = (*i)->Measure(ld.ForWidget(*this, *(*i)));
		}
		
		if(dim.x > minDim.x){
			minDim.x = dim.x;
		}
		if(dim.y > minDim.y){
			minDim.y = dim.y;
		}
	}
	
	minDim += this->Padding().lb + this->Padding().rt;
	return minDim;
}

