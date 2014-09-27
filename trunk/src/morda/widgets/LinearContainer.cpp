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
			
			if(const stob::Node* weight = (*i)->GetLayoutProperty(D_Weight)){
				info->weight = weight->AsFloat();
				netWeight += info->weight;
			}else{
				info->weight = 0;
			}
			
			Vec2b fill;
				
			if(const stob::Node* n = (*i)->GetLayoutProperty(Layout::D_Fill())){
				fill = Vec2bFromSTOB(n);
			}else{
				fill = Vec2b(false, false);
			}
			
			if(const  stob::Node* n = (*i)->GetLayoutProperty(LayoutDim::D_Dim())){
				LayoutDim dim = LayoutDim::FromSTOB(n);
				info->dim = dim.ForWidget(*(*i));
				
				if(fill[transIndex]){
					ting::util::ClampBottom(info->dim[transIndex], this->Rect().d[transIndex]);
				}
				
				info->dim = (*i)->Measure(info->dim);
			}else{
				info->dim = (*i)->GetMinDim();
			}
			
			if(const stob::Node* n = (*i)->GetLayoutProperty(Gravity::D_Gravity())){
				info->gravity = Gravity::FromSTOB(n);
			}else{
				info->gravity = this->gravity();
			}
			
			if(auto n = (*i)->GetLayoutProperty(Layout::D_Margins())){
				info->margins = LeftBottomRightTop::FromSTOB(n);
			}else{
				info->margins = LeftBottomRightTop::Default();
			}
			
			//if not first child then select largest margin
			if(info != infoArray.begin()){
				if(this->IsVertical()){
					info->margin = std::max((info - 1)->margins.lb.y, info->margins.rt.y);
				}else{
					info->margin = std::max((info - 1)->margins.rt.x, info->margins.lb.x);
				}
			}else{
				if(this->IsVertical()){
					info->margin = info->margins.rt.y;
				}else{
					info->margin = info->margins.lb.x;
				}
			}
			
			rigid += info->dim[longIndex] + info->margin;
		}
		if(this->IsVertical()){
			rigid += infoArray.rbegin()->margins.lb.y;
		}else{
			rigid += infoArray.rbegin()->margins.rt.x;
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
			
			ASSERT(netWeight >= 0)
			if(netWeight > 0){
				newSize[longIndex] += (info->weight / netWeight) * flexible;
			}

			Vec2r newPos;
			if(this->IsVertical()){
				newPos.y = this->Rect().d.y - pos - info->margin - newSize.y;
			}else{
				newPos.x = pos + info->margin;
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
	
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		LeftBottomRightTop margins;
		
		if(auto n = (*i)->GetLayoutProperty(Layout::D_Margins())){
			margins = LeftBottomRightTop::FromSTOB(n);
		}else{
			margins = LeftBottomRightTop::Default();
		}

		
		morda::Vec2r dim;
		
		if(auto n = (*i)->GetLayoutProperty(LayoutDim::D_Dim())){
			LayoutDim ld = LayoutDim::FromSTOB(n);
			dim = (*i)->Measure(ld.ForWidget(*(*i)));
		}else{
			dim = (*i)->GetMinDim();
		}
		
		
		ting::util::ClampBottom(minDim[transIndex], dim[transIndex] + margins.lb[transIndex] + margins.rt[transIndex]);
		
		minDim[longIndex] += dim[longIndex];
		
		if(i == this->Children().begin()){//if first widget
			if(this->IsVertical()){
				minDim.y += margins.rt.y;
			}else{
				minDim.x += margins.lb.x;
			}
		}else if((++decltype(i)(i)) == this->Children().end()){//if last widget
			if(this->IsVertical()){
				minDim.y += margins.lb.y;
			}else{
				minDim.x += margins.rt.x;
			}
		}else{//if not first child
			if(this->IsVertical()){
				minDim.y += std::max(prevMargin, margins.rt.y);
				prevMargin = margins.lb.y;
			}else{
				minDim.x += std::max(prevMargin, margins.lb.x);
				prevMargin = margins.rt.x;
			}
			
		}
	}
	
	return minDim;
}


