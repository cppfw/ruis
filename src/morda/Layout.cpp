#include "Layout.hpp"



using namespace morda;



namespace{

const char* D_Min = "min";
const char* D_Max = "max";
const char* D_Dim = "dim";

}//~namespace


//static
Layout::Dim Layout::Dim::FromSTOB(const stob::Node& node)throw(){
	Dim ret;
	
	const stob::Node* n = node.Child();
	if(!n){
		return Dim::Default();
	}
	
	for(unsigned i = 0; i != 2; ++i){
		if(n->Value() == D_Min){
			ret[i].unit = MIN;
		}else if(n->Value() == D_Max){
			ret[i].unit = MAX;
		}else if(n->Value().length() > 0 && n->Value()[n->Value().length() - 1] == '%'){
			ret[i].unit = FRACTION;
			ret[i].value = n->AsFloat() / 100;
		}else{
			ret[i].unit = PIXEL;
			ret[i].value = n->AsFloat();
		}
		
		n = n->Next();
		if(!n){
			ret[i].unit = MIN;
			break;
		}
	}
	
	return ret;
}



//static
Layout::Dim Layout::Dim::FromLayout(const stob::Node& layout)throw(){
	const stob::Node* dim = layout.Child(D_Dim).second;
	if(!dim){
		return Dim::Default();
	}
	return Dim::FromSTOB(*dim);
}



//static
Layout::Dim Layout::Dim::FromPropLayout(const stob::Node& prop)throw(){
	const stob::Node* layout = prop.Child(D_Layout()).second;
	if(!layout){
		return Dim::Default();
	}
	return Dim::FromLayout(*layout);
}
