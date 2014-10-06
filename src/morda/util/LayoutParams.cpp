#include "LayoutParams.hpp"

#include "util.hpp"

#include "../widgets/Widget.hpp"


using namespace morda;



DimLayoutParams::DimLayoutParams(const stob::Node* chain){
	if(auto n = GetProperty(chain, "dim")){
		this->dim = DimVec2rFromSTOB(n);
	}else{
		this->dim = Vec2r(-1, -1);
	}
	
	if(auto n = GetProperty(chain, "fill")){
		this->fill = Vec2bFromSTOB(n);
	}else{
		this->fill = Vec2b(false, false);
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
