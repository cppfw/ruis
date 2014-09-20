#include "Gravity.hpp"
#include "../util/util.hpp"


using namespace morda;



Vec2r Gravity::PosForRect(const Vec2r& parentDim, const Vec2r& dim)const NOEXCEPT{
	Vec2r ret;
	
	for(unsigned i = 0; i != 2; ++i){
		float noDim = parentDim[i] - dim[i];
		if(noDim <= 0){
			ret[i] = (parentDim[i] - dim[i]) / 2;
			continue;
		}
		
		ret[i] = noDim * (*this)[i];
	}
	
	return ret.Rounded();
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
