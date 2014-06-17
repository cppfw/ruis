#include "LinearContainer.hpp"

#include "../util/Gravity.hpp"
#include "../util/LayoutDim.hpp"
#include "../util/util.hpp"


using namespace morda;



namespace{
	
const char* D_Margins = "margins";
const char* D_Weight = "weight";

class Info{
public:
	float weight;
	Vec2f dim;
	float margin;//actual margin between child widgets
	Gravity gravity;
	LeftBottomRightTop margins;
};

}//~namespace



LinearContainer::LinearContainer(const stob::Node& desc) :
		Widget(desc),
		Container(desc),
		PaddedWidget(desc),
		LinearWidget(desc)
{}



//override
void LinearContainer::OnResize(){
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	ting::Array<Info> info(this->NumChildren());
	
	//Calculate rigid size, net weight and store weights and margins
	float rigid = this->Padding()[longIndex] + this->Padding()[2 + longIndex];
	float netWeight = 0;
	
	{
		Info* i = info.Begin();
		for(const ting::Ref<Widget>* c = &this->Children(); *c; c = &(*c)->Next(), ++i){
			ASSERT(info.Overlaps(i))

			const stob::Node* layout = (*c)->GetProperty("layout");//TODO: const str

			if(!layout){
				i->dim = (*c)->GetMinDim();
				i->weight = 0;
				i->gravity = Gravity::Default();
				i->margins = LeftBottomRightTop::Default();
			}else{
				if(const stob::Node* weight = layout->GetProperty(D_Weight)){
					i->weight = weight->AsFloat();
					netWeight += i->weight;
				}else{
					i->weight = 0;
				}

				LayoutDim dim = LayoutDim::FromLayout(*layout);
				i->dim = dim.ForWidget(*(*c));
				
				i->gravity = Gravity::FromLayout(*layout);
				
				if(const stob::Node* margins = layout->Child(D_Margins).node()){
					i->margins = LeftBottomRightTop::FromSTOB(*margins);
				}else{
					i->margins = LeftBottomRightTop::Default();
				}
			}
			
			if(i != info.Begin()){//if not first child
				i->margin = std::max(
						(i - 1)->margins[this->IsReverse() ? longIndex : (longIndex + 2)],
						i->margins[this->IsReverse() ? (longIndex + 2) : longIndex]
					);
			}else{
				i->margin = 0;
			}
			
			rigid += i->dim[longIndex] + i->margin;
		}
	}
	
	//arrange widgets
	{
		float flexible = this->Rect().d[longIndex] - rigid;
		ting::util::ClampBottom(flexible, 0.0f);
		ASSERT(flexible >= 0)
		
		float pos = this->Padding()[this->IsReverse() ? (longIndex + 2) : longIndex];//start arranging widgets from padding
		Info *i = info.Begin();
		for(const ting::Ref<Widget>* c = &this->Children(); *c; c = &(*c)->Next(), ++i){
			Vec2f newSize(i->dim);
			
			if(netWeight > 0){
				newSize[longIndex] += (i->weight / netWeight) * flexible;
			}

			Vec2f newPos;
			if((this->IsVertical() && !this->IsReverse()) || (!this->IsVertical() && this->IsReverse())){
				newPos[longIndex] = this->Rect().d[longIndex] - pos - i->margin - newSize[longIndex];
			}else{
				newPos[longIndex] = pos + i->margin;
			}
			pos += i->margin + newSize[longIndex];

			(*c)->Resize(RoundVec(newSize));
			
			newPos[transIndex] = i->gravity.PosForRect(*this, (*c)->Rect().d)[transIndex];
			
			(*c)->MoveTo(RoundVec(newPos));
		}
	}
}



//override
morda::Vec2f LinearContainer::ComputeMinDim()const throw(){
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	morda::Vec2f minDim(0);
	
	float prevMargin = 0;
	
	for(const ting::Ref<const Widget>* c = &this->Children(); *c; c = &(*c)->Next()){
		LeftBottomRightTop margins = LeftBottomRightTop::Default();
		morda::Vec2f dim = (*c)->GetMinDim();
		if((*c)->Prop()){
			if(const stob::Node* layout = (*c)->Prop()->Child("layout").node()){//TODO: layout str
				if(const stob::Node* m = layout->Child(D_Margins).node()){
					margins = LeftBottomRightTop::FromSTOB(*m);
				}
				
				{
					LayoutDim ld = LayoutDim::FromLayout(*layout);
					
					//FRACTION is not allowed when computing min size. Change it to MIN.
					for(unsigned i = 0; i != 2; ++i){
						if(ld[i].unit == LayoutDim::FRACTION){
							ld[i].unit = LayoutDim::MIN;
						}
					}
					
					dim = ld.ForWidget(*(*c));
				}
			}
		}
		
		if(minDim[transIndex] < dim[transIndex]){
			minDim[transIndex] = dim[transIndex];
		}
		minDim[longIndex] += dim[longIndex];
		
		//margin works for non-first children only
		if((*c)->Prev().IsValid()){//if not first child
			minDim[longIndex] += std::max(
					prevMargin,
					margins[this->IsReverse() ? (longIndex + 2) : longIndex]
				);
		}
		
		prevMargin = margins[this->IsReverse() ? longIndex : (longIndex + 2)];
	}
	
	minDim[0] += this->Padding()[0] + this->Padding()[2];
	minDim[1] += this->Padding()[1] + this->Padding()[3];
	
	return minDim;
}


