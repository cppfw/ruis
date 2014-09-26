#include "LinearContainer.hpp"

#include "../util/Layout.hpp"
#include "../util/Gravity.hpp"
#include "../util/LayoutDim.hpp"
#include "../util/util.hpp"
#include "../util/LeftBottomRightTop.hpp"

#include <cmath>



using namespace morda;


//TODO: add edge margins


namespace{
	
const char* D_Margins = "margins";
const char* D_Weight = "weight";


class Info{
public:
	real weight;
	Vec2r dim;
	real margin;//actual margin between child widgets
	Gravity gravity;
	LeftBottomRightTop margins;
};

}//~namespace



LinearContainer::LinearContainer(const stob::Node* desc) :
		Widget(desc),
		Container(desc),
		LinearWidget(desc),
		GravitatingWidget(desc)
{}



//override
void LinearContainer::OnResize(){
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	std::vector<Info> infoArray(this->Children().size());
	
	//Calculate rigid size, net weight and store weights and margins
	real rigid = 0;
	real netWeight = 0;
	
	{
		auto info = infoArray.begin();
		for(auto i = this->Children().cbegin(); i != this->Children().cend(); ++i, ++info){
			ASSERT(info != infoArray.end())
			if(const stob::Node* layout = (*i)->GetPropertyNode(Container::D_Layout())){
				if(const stob::Node* weight = layout->GetProperty(D_Weight)){
					info->weight = weight->AsFloat();
					netWeight += info->weight;
				}else{
					info->weight = 0;
				}
				
				morda::Vector2<bool> fill;
				
				if(const stob::Node* n = layout->GetProperty(Layout::D_Fill())){
					fill = TwoBoolsFromSTOB(n);
				}else{
					fill = morda::Vector2<bool>(false, false);
				}
				
				LayoutDim dim = LayoutDim::FromLayout(*layout);
				info->dim = dim.ForWidget(*(*i));
				
				if(fill[transIndex]){
					ting::util::ClampBottom(info->dim[transIndex], this->Rect().d[transIndex]);
				}
				
				info->dim = (*i)->Measure(info->dim);
				
				if(const stob::Node* g = layout->Child(Gravity::D_Gravity()).node()){
					info->gravity = Gravity::FromSTOB(*g);
				}else{
					info->gravity = this->gravity();
				}
				
				if(const stob::Node* margins = layout->Child(D_Margins).node()){
					info->margins = LeftBottomRightTop::FromSTOB(*margins);
				}else{
					info->margins = LeftBottomRightTop::Default();
				}
			}else{
				info->dim = (*i)->GetMinDim();
				info->weight = 0;
				info->gravity = this->gravity();
				info->margins = LeftBottomRightTop::Default();
			}
			
			if(info != infoArray.begin()){//if not first child
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
		real flexible = this->Rect().d[longIndex] - rigid;
		ting::util::ClampBottom(flexible, 0.0f);
		if(!std::isfinite(flexible)){
			flexible = 0;
		}
		ASSERT_INFO(flexible >= 0, "flexible = " << flexible)
		
		real pos = 0;
		auto info = infoArray.begin();
		for(auto i = this->Children().begin(); i != this->Children().end(); ++i, ++info){
			Vec2r newSize(info->dim);
			
			if(netWeight > 0){
				newSize[longIndex] += (info->weight / netWeight) * flexible;
			}

			Vec2r newPos;
			if((this->IsVertical() && !this->IsReverse()) || (!this->IsVertical() && this->IsReverse())){
				newPos[longIndex] = this->Rect().d[longIndex] - pos - info->margin - newSize[longIndex];
			}else{
				newPos[longIndex] = pos + info->margin;
			}
			pos += info->margin + newSize[longIndex];

			(*i)->Resize(newSize.Rounded());
			
			newPos[transIndex] = info->gravity.PosForRect(this->Rect().d, (*i)->Rect().d)[transIndex];
			
			(*i)->MoveTo(newPos);
		}
	}
}



//override
morda::Vec2r LinearContainer::ComputeMinDim()const NOEXCEPT{
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	morda::Vec2r minDim(0);
	
	float prevMargin = 0;
	
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		LeftBottomRightTop margins = LeftBottomRightTop::Default();
		morda::Vec2r dim = (*i)->GetMinDim();
		if(const stob::Node* layout = (*i)->GetPropertyNode(Container::D_Layout())){
			if(const stob::Node* m = layout->Child(D_Margins).node()){
				margins = LeftBottomRightTop::FromSTOB(*m);
			}

			{
				LayoutDim ld = LayoutDim::FromLayout(*layout);

				dim = (*i)->Measure(ld.ForWidget(*(*i)));
			}
		}else{
			 dim = (*i)->GetMinDim();
		}
		
		ting::util::ClampBottom(minDim[transIndex], dim[transIndex]);
		
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
	
	return minDim;
}


