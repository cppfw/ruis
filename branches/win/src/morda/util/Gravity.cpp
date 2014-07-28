#include "Gravity.hpp"
#include "../util/util.hpp"


using namespace morda;



Vec2f Gravity::PosForRect(const PaddedWidget& w, const Vec2f& dim)const throw(){
	return PosForRect(w.Rect().d, w.Padding(), dim);
}



Vec2f Gravity::PosForRect(const Vec2f& parentDim, const LeftBottomRightTop& padding, const Vec2f& dim)const throw(){
	Vec2f noPaddings = parentDim - padding.lb - padding.rt;
	
	Vec2f ret;
	
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
Gravity Gravity::FromSTOB(const stob::Node& gravity)throw(){
	return Vec2fFromSTOB(gravity.Child()) / 100;
}



//static
Gravity Gravity::FromLayout(const stob::Node& layout)throw(){
	const stob::Node* gravity = layout.Child(D_Gravity()).node();
	if(!gravity){
		return Gravity::Default();
	}

	return Gravity::FromSTOB(*gravity);
}
