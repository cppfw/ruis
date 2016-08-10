/* 
 * File:   ResGradient.cpp
 * Author: ivan
 * 
 * Created on April 29, 2016, 11:53 AM
 */

#include "ResGradient.hpp"

#include "../util/util.hpp"



using namespace morda;



ResGradient::ResGradient(const std::vector<std::tuple<real,std::uint32_t> >& stops, bool vertical){
	for(auto& s : stops){
		this->colors.push_back(std::get<1>(s));
		this->colors.push_back(std::get<1>(s));
		
//		TRACE(<< "put color = " << std::hex << this->colors.back() << std::endl)
		
		if(vertical){
			this->vertices.push_back(kolme::Vec2f(0, std::get<0>(s)));
			this->vertices.push_back(kolme::Vec2f(1, std::get<0>(s)));
		}else{ ASSERT(!vertical)
			this->vertices.push_back(kolme::Vec2f(std::get<0>(s), 1));
			this->vertices.push_back(kolme::Vec2f(std::get<0>(s), 0));
		}
	}
	ASSERT(this->vertices.size() == this->colors.size())
}



std::shared_ptr<ResGradient> ResGradient::load(const stob::Node& chain, const papki::File& fi) {
	bool vertical;
	
	if(auto n = getProperty(&chain, "vertical")){
		vertical = n->asBool();
	}else{
		vertical = false;
	}
	
	std::vector<std::tuple<real,std::uint32_t>> stops;
	
	const char* stop_c = "Stop";
	for(auto n = chain.thisOrNext(stop_c).node(); n; n = n->next(stop_c).node()){
		real pos;
		if(auto p = getProperty(n->child(), "pos")){
			pos = p->asFloat();
		}else{
			pos = 0;
		}
		
		std::uint32_t color;
		if(auto p = getProperty(n->child(), "color")){
			color = p->asUint32();
		}else{
			color = 0xffffffff;
		}
		
		stops.push_back(std::make_tuple(pos, color));
	}
	
	return utki::makeShared<ResGradient>(stops, vertical);
}


void ResGradient::render(ClrPosShader& s) const {
	ASSERT(this->vertices.size() == this->colors.size())
	
//	TRACE(<< "matrix = " << matrix << std::endl)
	
	s.render(utki::wrapBuf(this->vertices), utki::wrapBuf(this->colors), Render::Mode_e::TRIANGLE_STRIP);
}

