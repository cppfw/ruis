#include "LayoutDim.hpp"
#include "../widgets/Container.hpp"
#include "util.hpp"


using namespace morda;



namespace{

const char* D_Min = "min";

}//~namespace


//static
LayoutDim LayoutDim::FromSTOB(const stob::Node* chain)NOEXCEPT{
	LayoutDim ret;
	
	const stob::Node* n = chain;
	
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
