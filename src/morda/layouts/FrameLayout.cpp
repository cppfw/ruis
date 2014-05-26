#include "FrameLayout.hpp"

#include "../util/Gravity.hpp"
#include "../util/util.hpp"



using namespace morda;


//TODO:


//override
void FrameLayout::ArrangeWidgets(Container& cont)const{
	for(const ting::Ref<Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
		Layout::Dim dim;
		Gravity gravity;

		if(const stob::Node* layout = Layout::GetLayoutProp((*c)->Prop())){
			dim = Layout::Dim::FromSTOB(layout->Child(Layout::Dim::D_Dim()).node());
			gravity = Gravity::FromLayout(*layout);
		}else{
			dim = Layout::Dim::Default();
			gravity = Gravity::Default();
		}
		
		(*c)->Resize(RoundVec(dim.ForWidget(*(*c))));
		
		(*c)->MoveTo(RoundVec(gravity.PosForWidget(*(*c))));
	}
}



//override
morda::Vec2f FrameLayout::ComputeMinDim(const Container& cont)const throw(){
	morda::Vec2f minDim(0);
	
	for(const ting::Ref<const Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
		morda::Vec2f dim = (*c)->GetMinDim();
		if((*c)->Prop()){
			Layout::Dim ld = Layout::Dim::FromPropLayout(*(*c)->Prop());
			
			//FRACTION is not allowed when computing min size. Change it to MIN.
			for(unsigned i = 0; i != 2; ++i){
				if(ld[i].unit == Layout::Dim::FRACTION){
					ld[i].unit = Layout::Dim::MIN;
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
