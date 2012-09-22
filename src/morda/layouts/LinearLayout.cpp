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
//	unsigned longIndex, transIndex;
//	if(this->isVertical){
//		longIndex = 1;
//		transIndex = 0;
//	}else{
//		longIndex = 0;
//		transIndex = 1;
//	}
	
	//TODO:
}



//override
morda::Vec2f LinearLayout::ComputeMinimalDimensions(const Container& cont)const throw(){
	morda::Vec2f minDim(0);
	
	unsigned longIndex, transIndex;
	if(this->isVertical){
		longIndex = 1;
		transIndex = 0;
	}else{
		longIndex = 0;
		transIndex = 1;
	}
	
	for(const ting::Ref<const Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
		morda::Vec2f md = (*c)->ComputeMinimalDimensions();
		if(minDim[transIndex] < md[transIndex]){
			minDim[transIndex] = md[transIndex];
		}
		minDim[longIndex] =+ md[longIndex];
	}
	return minDim;
}
