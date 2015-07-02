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



namespace{

class Info{
public:
	Vec2r measuredDim;
};

}



void LinearContainer::OnResize(){
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	std::vector<Info> infoArray(this->Children().size());
	
	//Calculate rigid size, net weight and store weights
	real rigid = 0;
	real netWeight = 0;
	
	{
		auto info = infoArray.begin();
		for(auto i = this->Children().cbegin(); i != this->Children().cend(); ++i, ++info){
			auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);
			
			netWeight += lp.weight;
			
			ASSERT(lp.dim[longIndex] != LayoutParams::D_Max)
			
			Vec2r d = this->dimForWidget(**i, lp);
			info->measuredDim = d;
			
			rigid += d[longIndex];
		}
	}
	
	//arrange widgets
	{
		real flexible = this->Rect().d[longIndex] - rigid;
		
		real pos = 0;
		
		auto info = infoArray.begin();
		for(auto i = this->Children().begin(); i != this->Children().end(); ++i, ++info){
			auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);
			
			if(lp.weight != 0){
				ASSERT(lp.weight > 0)
				Vec2r d;
				d[longIndex] = info->measuredDim[longIndex];
				if(flexible > 0){
					ASSERT(netWeight > 0)
					d[longIndex] += flexible * lp.weight / netWeight;
				}
				
				if(lp.dim[transIndex] == LayoutParams::D_Max){
					d[transIndex] = this->Rect().d[transIndex];
				}else{
					if(lp.dim[transIndex] == LayoutParams::D_Min || lp.dim[transIndex] < 0){
						d[transIndex] = -1;
					}else{
						d[transIndex] = lp.dim[transIndex];
					}
					if(d.x < 0 || d.y < 0){
						Vec2r md = (*i)->measure(d);
						for(unsigned i = 0; i != md.size(); ++i){
							if(d[i] < 0){
								d[i] = md[i];
							}
						}
					}
				}
				(*i)->Resize(d.Rounded());
			}else{
				(*i)->Resize(info->measuredDim);
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



morda::Vec2r LinearContainer::onMeasure(const morda::Vec2r& quotum)const{
	unsigned longIndex = this->GetLongIndex();
	unsigned transIndex = this->GetTransIndex();
	
	std::vector<Info> infoArray(this->Children().size());
	
	//calculate rigid length
	real rigidLength = 0;
	real height = quotum[transIndex] >= 0 ? quotum[transIndex] : 0;
	real netWeight = 0;
	
	{
		auto info = infoArray.begin();
		for(auto i = this->Children().begin(); i != this->Children().end(); ++i, ++info){
			auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);

			netWeight += lp.weight;

			if(lp.dim[longIndex] == LayoutParams::D_Max){
				throw morda::Exc("LinearContainer::onMeasure(): mistake: max in longitudional direction");
			}

			Vec2r d;
			if(lp.dim[transIndex] == LayoutParams::D_Max){
				if(quotum[transIndex] >= 0){
					d[transIndex] = quotum[transIndex];
				}else{
					d[transIndex] = -1;
				}
			}else if(lp.dim[transIndex] == LayoutParams::D_Min || lp.dim[transIndex] < 0){
				d[transIndex] = -1;
			}else{
				d[transIndex] = lp.dim[transIndex];
			}

			ASSERT(lp.dim[longIndex] != LayoutParams::D_Max)
			if(lp.dim[longIndex] == LayoutParams::D_Min || lp.dim[longIndex] < 0){
				d[longIndex] = -1;
			}else{
				d[longIndex] = lp.dim[longIndex];
			}

			d = (*i)->measure(d);
			info->measuredDim = d;

			rigidLength += d[longIndex];

			if(lp.weight == 0){
				if(quotum[transIndex] < 0){
					ting::util::ClampBottom(height, d[transIndex]);
				}
			}
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
	
	{
		auto info = infoArray.begin();
		for(auto i = this->Children().begin(); i != this->Children().end(); ++i, ++info){
			auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);
			ASSERT(lp.weight >= 0)
			if(lp.weight == 0){
				continue;
			}

			ASSERT(netWeight > 0)

			Vec2r d;
			d[longIndex] = info->measuredDim[longIndex];
			
			if(flexLen > 0){
				d[longIndex] += flexLen * lp.weight / netWeight;
			}
			
			if(lp.dim[transIndex] == LayoutParams::D_Max){
				if(quotum[transIndex] >= 0){
					d[transIndex] = quotum[transIndex];
				}else{
					d[transIndex] = -1;
				}
			}else if(lp.dim[transIndex] == LayoutParams::D_Min || lp.dim[transIndex] < 0){
				d[transIndex] = -1;
			}else{
				d[transIndex] = lp.dim[transIndex];
			}
			
			d = (*i)->measure(d);
			if(quotum[transIndex] < 0){
				ting::util::ClampBottom(height, d[transIndex]);
			}
		}
	}
	
	ret[transIndex] = height;
	return ret;
}


