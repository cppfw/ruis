#include "Gravity.hpp"
#include "../Layout.hpp"


using namespace morda;



namespace{

const char* D_Gravity = "gravity";
const char* D_Left = "left";
const char* D_Right = "right";
const char* D_Top = "top";
const char* D_Bottom = "bottom";

}//~namespace



//static
Gravity Gravity::FromSTOB(const stob::Node& gravity)throw(){
	Gravity ret;
	
	const stob::Node* n = gravity.Child();
	if(!n){
		return Gravity::Default();
	}
	
	if(*n == D_Left){
		ret.h = LEFT;
	}else if(*n == D_Right){
		ret.h = RIGHT;
	}else{
		ret.h = CENTER;
	}
	
	n = n->Next();
	if(!n){
		ret.v = CENTER;
		return ret;
	}

	if(*n == D_Top){
		ret.v = TOP;
	}else if(*n == D_Bottom){
		ret.v = BOTTOM;
	}else{
		ret.v = CENTER;
	}
	
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
