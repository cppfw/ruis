#include "util.hpp"

#include "../App.hpp"

#include <ting/math.hpp>


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



morda::Vec2f morda::DistanceVec2fFromSTOB(const stob::Node* chain){
	unsigned i;
	morda::Vec2f ret;
	
	float v = 0;
	for(i = 0; i != 2 && chain; ++i, chain = chain->Next()){
		v = ParseDistanceValue(*chain);
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}



float morda::ParseDistanceValue(const stob::Node& n){
	//check if millimeters
	if(n.ValueLength() >= 2 && n.Value()[n.ValueLength() - 1] == 'm' && n.Value()[n.ValueLength() - 2] == 'm'){
		return ting::math::Round(n.AsFloat() * App::Inst().DotsPerMm());
	}
	
	return n.AsFloat();
}

