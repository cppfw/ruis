#include "util.hpp"


using namespace morda;



morda::Vec2f morda::Vec2fFromSTOB(const stob::Node* chain){
	unsigned i;
	morda::Vec2f ret;
	
	float v = 0;
	for(i = 0; i != 2 && chain; ++i, chain = chain->Next()){
		v = chain->AsFloat();
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}
