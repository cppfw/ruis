#include "FrameContainer.hpp"

#include "../util/LayoutDim.hpp"
#include "../util/Gravity.hpp"
#include "../util/util.hpp"

using namespace morda;



FrameContainer::FrameContainer(const stob::Node& desc) :
		Widget(desc),
		Container(desc),
		PaddedWidget(desc)
{}



void FrameContainer::OnResize() {
	for(const ting::Ref<Widget>* c = &this->Children(); *c; c = &(*c)->Next()){
		LayoutDim dim;
		Gravity gravity;

		if(const stob::Node* layout = (*c)->GetProperty("layout")){//TODO: const str
			dim = LayoutDim::FromSTOB(layout->Child(LayoutDim::D_Dim()).node());
			gravity = Gravity::FromLayout(*layout);
		}else{
			dim = LayoutDim::Default();
			gravity = Gravity::Default();
		}
		
		(*c)->Resize(RoundVec(dim.ForWidget(*(*c))));
		
		(*c)->MoveTo(RoundVec(gravity.PosForRect(*this, (*c)->Rect().d)));
	}
}



morda::Vec2f FrameContainer::ComputeMinDim()const{
	morda::Vec2f minDim(0);
	
	for(const ting::Ref<const Widget>* c = &this->Children(); *c; c = &(*c)->Next()){
		morda::Vec2f dim = (*c)->GetMinDim();
		if((*c)->Prop()){
			LayoutDim ld = LayoutDim::FromPropLayout(*(*c)->Prop());
			
			//FRACTION is not allowed when computing min size. Change it to MIN.
			for(unsigned i = 0; i != 2; ++i){
				if(ld[i].unit == LayoutDim::FRACTION){
					ld[i].unit = LayoutDim::MIN;
				}
			}

			dim = ld.ForWidget(*(*c));
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

