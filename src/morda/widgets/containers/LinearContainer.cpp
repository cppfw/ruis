#include "LinearContainer.hpp"

#include "../../util/util.hpp"

#include <cmath>



using namespace morda;


namespace{

class Info{
public:
	real weight;
	Vec2r dim;
	bool fill;
};

}//~namespace



LinearContainer::LayoutParams::LayoutParams(const stob::Node* chain) :
		DimLayoutParams(chain)
{
	if(auto n = GetProperty(chain, "weight")){
		this->weight = n->AsFloat();
	}else{
		this->weight = 0;
	}
}




LinearContainer::LinearContainer(const stob::Node* chain) :
		Widget(chain),
		Container(chain),
		LinearWidget(chain)
{}



void LinearContainer::OnResize(){
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	std::vector<Info> infoArray(this->Children().size());
	
	bool lessThanMin = this->Rect().d[longIndex] < this->GetMinDim()[longIndex];
	
	//Calculate rigid size, net weight and store weights
	real rigid = 0;
	real fillRigid = 0;
	real netWeight = 0;
	
	{
		auto info = infoArray.begin();
		for(auto i = this->Children().cbegin(); i != this->Children().cend(); ++i, ++info){
			ASSERT(info != infoArray.end())
			
			auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);
			
			info->weight = lp.weight;
			netWeight += info->weight;
			
			info->fill = lp.fill[longIndex];
			
			info->dim = lp.DimForWidget(**i);
			
			if(lp.fill[transIndex]){
				info->dim[transIndex] = this->Rect().d[transIndex];
			}
			
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

			if(this->IsReverse()){
				newPos[longIndex] = this->Rect().d[longIndex] - pos - newSize[longIndex];
			}else{
				newPos[longIndex] = pos;
			}
			
			pos += step;

			(*i)->Resize(newSize.Rounded());
			
			newPos[transIndex] = ting::math::Round((this->Rect().d[transIndex] - (*i)->Rect().d[transIndex]) / 2);
			
			(*i)->MoveTo(newPos);
		}
	}
}



morda::Vec2r LinearContainer::ComputeMinDim()const NOEXCEPT{
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	morda::Vec2r minDim(0);
	
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);
		
		morda::Vec2r dim = lp.DimForWidget(**i);
		
		ting::util::ClampBottom(minDim[transIndex], dim[transIndex]);
		
		minDim[longIndex] += dim[longIndex];
	}
	
	return minDim;
}


