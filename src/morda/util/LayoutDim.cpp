#include "LayoutDim.hpp"
#include "../widgets/Container.hpp"
#include "util.hpp"


using namespace morda;



namespace{

const char* D_Min = "min";
const char* D_Layout = "layout";

}//~namespace


//static
LayoutDim LayoutDim::FromSTOB(const stob::Node& node)NOEXCEPT{
	LayoutDim ret;
	
	const stob::Node* n = node.Child();
	
	for(unsigned i = 0; i != 2; ++i){
		if(!n){
			ret[i] = -1;
			continue;
		}
		
		if(*n == D_Min){
			ret[i] = -1;
		}else{
			ret[i] = DimValue(*n);
		}
		
		n = n->Next();
	}
	
	return ret;
}



//static
LayoutDim LayoutDim::FromLayout(const stob::Node& layout)NOEXCEPT{
	const stob::Node* dim = layout.Child(LayoutDim::D_Dim()).node();
	if(!dim){
		return LayoutDim::Default();
	}
	return LayoutDim::FromSTOB(*dim);
}



//static
LayoutDim LayoutDim::FromPropLayout(const stob::Node& prop)NOEXCEPT{
	const stob::Node* layout = prop.Child(D_Layout).node();
	if(!layout){
		return LayoutDim::Default();
	}
	return LayoutDim::FromLayout(*layout);
}



Vec2r LayoutDim::ForWidget(const Widget& w)const NOEXCEPT{
	Vec2r ret;
	
	for(unsigned i = 0; i != 2; ++i){
		const real& v = this->operator[](i);
		
		if(v < 0){
			ret[i] = w.GetMinDim()[i];
		}else{
			ret[i] = v;
		}
	}
	
	return ret.Rounded();
}
