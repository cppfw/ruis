#include "Gravity.hpp"
#include "../util/util.hpp"


using namespace morda;



namespace{

const char* D_Gravity = "gravity";

}//~namespace



Vec2f Gravity::PosForRect(const PaddedWidget& w, const Vec2f& dim)const throw(){
	return PosForRect(w.Rect().d, w.Padding(), dim);
}



Vec2f Gravity::PosForRect(const Vec2f& parentDim, const LeftBottomRightTop& padding, const Vec2f& dim)const throw(){
	return padding.lb + (parentDim - padding.lb - padding.rt - dim).CompMul(*this);
}



//static
Gravity Gravity::FromSTOB(const stob::Node& gravity)throw(){
	return Vec2fFromSTOB(gravity.Child()) / 100;
}



//static
Gravity Gravity::FromLayout(const stob::Node& layout)throw(){
	const stob::Node* gravity = layout.Child(D_Gravity).node();
	if(!gravity){
		return Gravity::Default();
	}

	return Gravity::FromSTOB(*gravity);
}
