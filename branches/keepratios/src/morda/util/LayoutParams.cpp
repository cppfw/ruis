#include "LayoutParams.hpp"

#include "util.hpp"

#include "../widgets/Widget.hpp"


using namespace morda;



DimLayoutParams::DimLayoutParams(const stob::Node* chain){
	if(auto n = GetProperty(chain, "dimX")){
		this->dim.x = DimValueFromSTOB(*n);
	}else{
		this->dim.x = -1;
	}
	
	if(auto n = GetProperty(chain, "dimY")){
		this->dim.y = DimValueFromSTOB(*n);
	}else{
		this->dim.y = -1;
	}
	
	if(auto n = GetProperty(chain, "fillX")){
		this->fill.x = n->AsBool();
	}else{
		this->fill.x = false;
	}
	
	if(auto n = GetProperty(chain, "fillY")){
		this->fill.y = n->AsBool();
	}else{
		this->fill.y = false;
	}
	
	if(auto n = GetProperty(chain, "keepRatio")){
		this->keepRatio = n->AsBool();
	}else{
		this->keepRatio = false;
	}
}



Vec2r DimLayoutParams::DimForWidget(const Widget& w)const NOEXCEPT{
	Vec2r ret;

	for(unsigned i = 0; i != 2; ++i){
		const real& v = this->dim[i];

		if(v < 0){
			ret[i] = w.GetMinDim()[i];
		}else{
			ret[i] = v;
		}
	}

	return ret;
}
