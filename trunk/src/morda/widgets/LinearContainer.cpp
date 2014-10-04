#include "LinearContainer.hpp"

#include "../util/Layout.hpp"
#include "../util/LayoutDim.hpp"
#include "../util/util.hpp"

#include <cmath>



using namespace morda;


namespace{

const char* D_Weight = "weight";


class Info{
public:
	real weight;
	Vec2r dim;
	bool fill;
};

}//~namespace



LinearContainer::LinearContainer(const stob::Node* desc) :
		Widget(desc),
		Container(desc),
		LinearWidget(desc)
{}



//override
void LinearContainer::OnResize(){
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	std::vector<Info> infoArray(this->Children().size());
	
	bool lessThanMin = this->Rect().d[longIndex] < this->GetMinDim()[longIndex];
	
	//Calculate rigid size, net weight and store weights and margins
	real rigid = 0;
	real fillRigid = 0;
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
			
			info->fill = fill[longIndex];
			
			if(const  stob::Node* n = (*i)->GetLayoutProperty(LayoutDim::D_Dim())){
				LayoutDim dim = LayoutDim::FromSTOB(n);
				info->dim = dim.ForWidget(*(*i));
			}else{
				info->dim = (*i)->GetMinDim();
			}
			
			if(fill[transIndex]){
				info->dim[transIndex] = this->Rect().d[transIndex];
			}
			
			info->dim = (*i)->Measure(info->dim);
			
			if(lessThanMin && info->fill){
				fillRigid += info->dim[longIndex];
			}
			
			rigid += info->dim[longIndex];
		}
	}
	
	//arrange widgets
	{		
		real flexible = this->Rect().d[longIndex] - rigid;
		lessThanMin = flexible < 0;
		ASSERT_INFO((!lessThanMin && flexible >= 0) || (lessThanMin && flexible < 0), "lessThanMin = " << lessThanMin << ", flexible = " << flexible)
		
		bool noSpaceAvailable = flexible + fillRigid < 0;//true if there is no space can be freed from 'fill' widgets
		
		real pos = 0;
		auto info = infoArray.begin();
		for(auto i = this->Children().begin(); i != this->Children().end(); ++i, ++info){
			Vec2r newSize(info->dim);
			
			real step = newSize[longIndex];
			
			if(flexible >= 0){
				ASSERT(netWeight >= 0)
				if(netWeight > 0){
					step += (info->weight / netWeight) * flexible;
				}
				
				if(info->fill){
					newSize[longIndex] = step;
				}
			}else if(info->fill){
				ASSERT(fillRigid >= 0)
//				TRACE(<< "noSpaceAvailable = " << noSpaceAvailable << ", flexible = " << flexible << ", fillRigid = " << fillRigid << std::endl)
				if(noSpaceAvailable){
					step = 0;
				}else if(fillRigid > 0){
					step += (info->dim[longIndex] / fillRigid) * flexible;
				}
				
				newSize[longIndex] = step;
			}
			
			Vec2r newPos;
			if(this->IsVertical()){
				newPos.y = this->Rect().d.y - pos - newSize.y;
			}else{
				newPos.x = pos;
			}
			
			pos += step;

			(*i)->Resize(newSize.Rounded());
			
			newPos[transIndex] = (this->Rect().d[transIndex] - (*i)->Rect().d[transIndex]) / 2;
			
			(*i)->MoveTo(newPos);
		}
	}
}



//override
morda::Vec2r LinearContainer::ComputeMinDim()const NOEXCEPT{
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	morda::Vec2r minDim(0);
	
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		morda::Vec2r dim;
		
		if(auto n = (*i)->GetLayoutProperty(LayoutDim::D_Dim())){
			LayoutDim ld = LayoutDim::FromSTOB(n);
			dim = (*i)->Measure(ld.ForWidget(*(*i)));
		}else{
			dim = (*i)->GetMinDim();
		}
		
		
		ting::util::ClampBottom(minDim[transIndex], dim[transIndex]);
		
		minDim[longIndex] += dim[longIndex];
	}
	
	return minDim;
}


