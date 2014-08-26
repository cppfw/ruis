#include "Gravity.hpp"
#include "../util/util.hpp"


using namespace morda;



Vec2r Gravity::PosForRect(const PaddedWidget& w, const Vec2r& dim)const NOEXCEPT{
	return PosForRect(w.Rect().d, w.Padding(), dim);
}



Vec2r Gravity::PosForRect(const Vec2r& parentDim, const LeftBottomRightTop& padding, const Vec2r& dim)const NOEXCEPT{
	Vec2r noPaddings = parentDim - padding.lb - padding.rt;
	
	Vec2r ret;
	
	for(unsigned i = 0; i != 2; ++i){
		if(noPaddings[i] <= 0){
			float paddingRatio = padding.lb[i] / (padding.lb[i] + padding.rt[i]);
			ret[i] = parentDim[i] * paddingRatio - dim[i] / 2;
			continue;
		}
		
		float noDim = noPaddings[i] - dim[i];
		if(noDim <= 0){
			ret[i] = padding.lb[i] + (noPaddings[i] - dim[i]) / 2;
			continue;
		}
		
		ret[i] = padding.lb[i] + noDim * (*this)[i];
	}
	
	return RoundVec(ret);
}



//static
Gravity Gravity::FromSTOB(const stob::Node& gravity)NOEXCEPT{
	return Vec2rFromSTOB(gravity.Child()) / 100;
}



//static
Gravity Gravity::FromLayout(const stob::Node& layout)NOEXCEPT{
	const stob::Node* gravity = layout.Child(D_Gravity()).node();
	if(!gravity){
		return Gravity::Default();
	}

	return Gravity::FromSTOB(*gravity);
}
