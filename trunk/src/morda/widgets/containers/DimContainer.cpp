#include "DimContainer.hpp"
#include "../../util/LayoutParams.hpp"

using namespace morda;


DimContainer::DimContainer(const stob::Node* chain) :
		Widget(chain),
		Container(chain)
{
}



void DimContainer::OnResize() {
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<DimLayoutParams>(**i);
		
		Vec2r d = lp.DimForWidget(**i);
		for(unsigned i = 0; i != 2; ++i){
			if(!lp.fill[i]){
				continue;
			}
			d[i] = this->Rect().d[i];
		}
		
		(*i)->Resize(d);
	}
}

morda::Vec2r DimContainer::ComputeMinDim() const {
	morda::Vec2r minDim(0);
	
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<DimLayoutParams>(**i);
		
		morda::Vec2r d = lp.DimForWidget(**i) + (*i)->Rect().p;
		
		ting::util::ClampBottom(minDim.x, d.x);
		ting::util::ClampBottom(minDim.y, d.y);
	}
	
	return minDim;
}



std::unique_ptr<LayoutParams> DimContainer::CreateLayoutParams(const stob::Node* chain) const {
	return DimLayoutParams::New(chain);
}
