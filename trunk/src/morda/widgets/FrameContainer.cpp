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

		if(const stob::Node* layout = (*i)->GetProperty(Container::D_Layout())){
			dim = LayoutDim::FromSTOB(layout->Child(LayoutDim::D_Dim()).node());
			gravity = Gravity::FromLayout(*layout);
		}else{
			dim = LayoutDim::Default();
			gravity = Gravity::Default();
		}
		
		(*i)->Resize(RoundVec(dim.ForWidget(*this, *(*i))));
		
		(*i)->MoveTo(RoundVec(gravity.PosForRect(*this, (*i)->Rect().d)));
	}
}



morda::Vec2f FrameContainer::ComputeMinDim()const{
	morda::Vec2f minDim(0);
	
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		morda::Vec2f dim = (*i)->GetMinDim();
		if((*i)->Prop()){
			LayoutDim ld = LayoutDim::FromPropLayout(*(*i)->Prop());
			
			//FRACTION is not allowed when computing min size. Change it to MIN.
			for(unsigned j = 0; j != 2; ++j){
				if(ld[j].unit == LayoutDim::FRACTION){
					ld[j].unit = LayoutDim::MIN;
				}
			}

			dim = ld.ForWidget(*this, *(*i));
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

