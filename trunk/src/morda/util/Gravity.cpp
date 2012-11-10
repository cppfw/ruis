#include "Gravity.hpp"
#include "../Layout.hpp"
#include "../util/util.hpp"


using namespace morda;



namespace{

const char* D_Gravity = "gravity";

}//~namespace



Vec2f Gravity::PosForWidget(const Widget& w)const throw(){
	ting::Ref<Container> p = w.Parent();
	if(!p){
		return Vec2f(0);
	}
	
	Vec2f ret;
	
	ret.x = p->Padding().lt.x + (p->Rect().d.x - p->Padding().lt.x - p->Padding().rb.x - w.Rect().d.x) * this->x;
	//TODO: for y
	
	return ret;
}



//static
Gravity Gravity::FromSTOB(const stob::Node& gravity)throw(){
	Gravity ret = Vec2fFromSTOB(gravity.Child()) / 100;
	
	return ret;
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
