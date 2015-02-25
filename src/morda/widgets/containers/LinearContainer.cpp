#include "LinearContainer.hpp"

#include "../../util/util.hpp"

#include <cmath>



using namespace morda;



LinearContainer::LayoutParams::LayoutParams(const stob::Node* chain) :
		Container::LayoutParams(chain)
{
	if(auto n = GetProperty(chain, "weight")){
		this->weight = n->AsFloat();
	}else{
		this->weight = 0;
	}
}




LinearContainer::LinearContainer(bool isVertical, const stob::Node* chain) :
		Widget(chain),
		Container(chain),
		isVertical(isVertical)
{}



void LinearContainer::OnResize(){
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	//Calculate rigid size, net weight and store weights
	real rigid = 0;
	real netWeight = 0;
	
	{
		for(auto i = this->Children().cbegin(); i != this->Children().cend(); ++i){
			auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);
			
			netWeight += lp.weight;
			
			if(lp.weight != 0){
				continue;
			}
			
			Vec2r d;
			d[longIndex] = lp.dim[longIndex];
			if(lp.fill[transIndex]){
				d[transIndex] = this->Rect().d[transIndex];
			}else{
				d[transIndex] = lp.dim[transIndex];
			}
			
			d = (*i)->measure(d);
			
			rigid += d[longIndex];
			
			if(lp.fill[transIndex]){
				d[transIndex] = this->Rect().d[transIndex];
			}
			
			(*i)->Resize(d);
		}
	}
	
	//arrange widgets
	{
		real flexible = this->Rect().d[longIndex] - rigid;
		
		real pos = 0;
		
		for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
			auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);
			
			if(lp.weight != 0){
				Vec2r d;
				if(flexible < 0){
					d[longIndex] = 0;
				}else{
					d[longIndex] = flexible * lp.weight / netWeight;
				}
				if(lp.fill[transIndex]){
					d[transIndex] = this->Rect().d[transIndex];
				}else{
					d[transIndex] = lp.dim[transIndex];
					d = (*i)->measure(d);
				}
				(*i)->Resize(d.Rounded());
			}
			
			Vec2r newPos;

			if(this->isVertical){
				newPos[longIndex] = this->Rect().d[longIndex] - pos - (*i)->Rect().d[longIndex];
			}else{
				newPos[longIndex] = pos;
			}
			
			pos += (*i)->Rect().d[longIndex];
			
			newPos[transIndex] = ting::math::Round((this->Rect().d[transIndex] - (*i)->Rect().d[transIndex]) / 2);
			
			(*i)->MoveTo(newPos);
		}
	}
}



morda::Vec2r LinearContainer::onMeasure(const morda::Vec2r& quotum)const NOEXCEPT{
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	
	//calculate rigid length
	real rigidLength = 0;
	real height = quotum[transIndex] >= 0 ? quotum[transIndex] : 0;
	real netWeight = 0;
	
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);
		
		if(lp.weight != 0){
			netWeight += lp.weight;
			continue;
		}
		
		if(lp.fill[longIndex]){
			//throw morda::Exc("LinearContainer::onMeasure(): mistake: fill in longitudional direction");
			continue;
		}
		
		Vec2r d;
		if(quotum[transIndex] >= 0 && lp.fill[transIndex]){
			d[transIndex] = quotum[transIndex];
		}else{
			d[transIndex] = lp.dim[transIndex];
		}
		
		ASSERT(!lp.fill[longIndex])
		d[longIndex] = lp.dim[longIndex];
		
		d = (*i)->measure(d);
		
		rigidLength += d[longIndex];
		
		if(quotum[transIndex] < 0){
			ting::util::ClampBottom(height, d[transIndex]);
		}
	}
	
	Vec2r ret;
	
	real flexLen;
	
	if(quotum[longIndex] < 0){
		ret[longIndex] = rigidLength;
		flexLen = 0;
	}else{
		ret[longIndex] = quotum[longIndex];
		flexLen = quotum[longIndex] - rigidLength;
	}
	
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);
		ASSERT(lp.weight >= 0)
		if(lp.weight == 0){
			continue;
		}
		
		ASSERT(netWeight > 0)
		
		Vec2r d;
		if(flexLen < 0){
			d[longIndex] = 0;
		}else{
			d[longIndex] = flexLen * lp.weight / netWeight;
		}
		d[transIndex] = lp.dim[transIndex];
		d = (*i)->measure(d);
		if(quotum[transIndex] < 0){
			ting::util::ClampBottom(height, d[transIndex]);
		}
	}
	
	ret[transIndex] = height;
	return ret;
}


