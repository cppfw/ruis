#include "LeftBottomRightTop.hpp"


using namespace morda;



//static
LeftBottomRightTop LeftBottomRightTop::FromSTOB(const stob::Node* chain)NOEXCEPT{
	LeftBottomRightTop ret;
	
	const stob::Node* n = chain;
	unsigned i = 0;
	float curVal = 0;
	for(; i != 4 && n; ++i, n = n->Next()){
		curVal = n->AsFloat();
		ret[i] = curVal;
	}
	
	for(; i != 4; ++i){
		ret[i] = curVal;
	}
	
	return ret;
}
