#include "Gravity.hpp"
#include "../util/util.hpp"


using namespace morda;



Vec2r Gravity::PosForRect(const Vec2r& parentDim, const Vec2r& dim, const LeftBottomRightTop& margins)const NOEXCEPT{
	Vec2r ret;
	
	for(unsigned i = 0; i != 2; ++i){
		float noDim = parentDim[i] - dim[i] - margins.lb[i] - margins.rt[i];
		if(noDim < 0){
			ret[i] = (parentDim[i] - dim[i]) / 2;
			continue;
		}
		
		ret[i] = noDim * (*this)[i] + margins.lb[i];
	}
	
	return ret.Rounded();
}



//static
Gravity Gravity::FromSTOB(const stob::Node* chain)NOEXCEPT{
	return Vec2rFromSTOB(chain) / 100;
}

