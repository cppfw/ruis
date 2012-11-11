#include "Gravity.hpp"
#include "../Layout.hpp"
#include "../util/util.hpp"


using namespace morda;



namespace{

const char* D_Gravity = "gravity";

}//~namespace



Vec2f Gravity::PosForWidget(const Widget& w)const throw(){
	ting::Ref<const Container> p = w.Parent();
	if(!p){
		return Vec2f(0);
	}
	
	return PosForRect(*p, w.Rect().d);
}



Vec2f Gravity::PosForRect(const Padded& w, const Vec2f& dim)const throw(){
	return w.Padding().lb + (w.Rect().d - w.Padding().lb - w.Padding().rt - dim).CompMul((*this));
}



//static
Gravity Gravity::FromSTOB(const stob::Node& gravity)throw(){
	return Vec2fFromSTOB(gravity.Child()) / 100;
}



//static
Gravity Gravity::FromLayout(const stob::Node& layout)throw(){
	const stob::Node* gravity = layout.Child(D_Gravity).second;
	if(!gravity){
		return Gravity::Default();
	}

	return Gravity::FromSTOB(*gravity);
}



//static
Gravity Gravity::FromPropLayout(const stob::Node& prop)throw(){
	const stob::Node* layout = Layout::GetLayoutProp(prop);
	if(!layout){
		return Gravity::Default();
	}
	
	return Gravity::FromLayout(*layout);
}
