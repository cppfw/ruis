#include "LinearLayout.hpp"



using namespace morda;



LinearLayout::LinearLayout(const stob::Node& description){
	if(const stob::Node* n = description.GetProperty("orientation")){
		if(n->Value() == "horizontal"){
			this->isVertical = false;
		}else{
			this->isVertical = true;
		}
	}else{
		this->isVertical = true;
	}
}



//override
void LinearLayout::ArrangeWidgets(Container& cont)const{
	
	//TODO:
}



//override
morda::Vec2f LinearLayout::ComputeMinimalDimensions(const Container& cont)const throw(){
	morda::Vec2f minDim(0);
	
	if(this->isVertical){
		for(const ting::Ref<const Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
			morda::Vec2f md = (*c)->ComputeMinimalDimensions();
			if(minDim.x < md.x){
				minDim.x = md.x;
			}
			minDim.y =+ md.y;
		}
	}else{
		for(const ting::Ref<const Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
			morda::Vec2f md = (*c)->ComputeMinimalDimensions();
			if(minDim.y < md.y){
				minDim.y = md.y;
			}
			minDim.x =+ md.x;
		}
	}
	return minDim;
}
