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
	
	ting::Array<Info> infoArray(this->Children().size());
	
	//Calculate rigid size, net weight and store weights and margins
	float rigid = this->Padding()[longIndex] + this->Padding()[2 + longIndex];
	float netWeight = 0;
	
	{
		Info* info = infoArray.Begin();
		for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i, ++info){
			ASSERT(infoArray.Overlaps(info))

			const stob::Node* layout = (*i)->GetProperty(Container::D_Layout());

			if(!layout){
				info->dim = (*i)->GetMinDim();
				info->weight = 0;
				info->gravity = Gravity::Default();
				info->margins = LeftBottomRightTop::Default();
			}else{
				if(const stob::Node* weight = layout->GetProperty(D_Weight)){
					info->weight = weight->AsFloat();
					netWeight += info->weight;
				}else{
					info->weight = 0;
				}

				LayoutDim dim = LayoutDim::FromLayout(*layout);
				info->dim = (*i)->Measure(dim.ForWidget(*this, *(*i)));
				
				info->gravity = Gravity::FromLayout(*layout);
				
				if(const stob::Node* margins = layout->Child(D_Margins).node()){
					info->margins = LeftBottomRightTop::FromSTOB(*margins);
				}else{
					info->margins = LeftBottomRightTop::Default();
				}
			}
			
			if(info != infoArray.Begin()){//if not first child
				info->margin = std::max(
						(info - 1)->margins[this->IsReverse() ? longIndex : (longIndex + 2)],
						info->margins[this->IsReverse() ? (longIndex + 2) : longIndex]
					);
			}else{
				info->margin = 0;
			}
			
			rigid += info->dim[longIndex] + info->margin;
		}
	}
	
	//arrange widgets
	{
		float flexible = this->Rect().d[longIndex] - rigid;
		ting::util::ClampBottom(flexible, 0.0f);
		ASSERT(flexible >= 0)
		
		float pos = this->Padding()[this->IsReverse() ? (longIndex + 2) : longIndex];//start arranging widgets from padding
		Info *info = infoArray.Begin();
		for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i, ++info){
			Vec2f newSize(info->dim);
			
			if(netWeight > 0){
				newSize[longIndex] += (info->weight / netWeight) * flexible;
			}

			Vec2f newPos;
			if((this->IsVertical() && !this->IsReverse()) || (!this->IsVertical() && this->IsReverse())){
				newPos[longIndex] = this->Rect().d[longIndex] - pos - info->margin - newSize[longIndex];
			}else{
				newPos[longIndex] = pos + info->margin;
			}
			pos += info->margin + newSize[longIndex];

			(*i)->Resize(RoundVec(newSize));
			
			newPos[transIndex] = info->gravity.PosForRect(*this, (*i)->Rect().d)[transIndex];
			
			(*i)->MoveTo(newPos);
		}
	}
}



//override
morda::Vec2f LinearContainer::ComputeMinDim()const throw(){
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	morda::Vec2f minDim(0);
	
	float prevMargin = 0;
	
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		LeftBottomRightTop margins = LeftBottomRightTop::Default();
		morda::Vec2f dim = (*i)->GetMinDim();
		if((*i)->Prop()){
			if(const stob::Node* layout = (*i)->Prop()->Child(Container::D_Layout()).node()){
				if(const stob::Node* m = layout->Child(D_Margins).node()){
					margins = LeftBottomRightTop::FromSTOB(*m);
				}
				
				{
					LayoutDim ld = LayoutDim::FromLayout(*layout);
					
					//FRACTION min size is 0
					for(unsigned j = 0; j != 2; ++j){
						if(ld[j].unit == LayoutDim::FRACTION){
							ld[j].unit = LayoutDim::PIXEL;
							ld[j].value = 0;
						}
					}
					
					dim = (*i)->Measure(ld.ForWidget(*this, *(*i)));
				}
			}
		}
		
		if(minDim[transIndex] < dim[transIndex]){
			minDim[transIndex] = dim[transIndex];
		}
		minDim[longIndex] += dim[longIndex];
		
		//margin works for non-first children only
		if(i != this->Children().begin()){//if not first child
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


