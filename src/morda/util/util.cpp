#include "util.hpp"

#include "../App.hpp"

#include <utki/debug.hpp>


using namespace morda;



morda::Vec2r morda::makeVec2rFromSTOB(const stob::Node* chain){
	unsigned i;
	morda::Vec2r ret;
	
	float v = 0;
	for(i = 0; i != 2 && chain; ++i, chain = chain->next()){
		v = chain->asFloat();
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}


Rectr morda::makeRectrFromSTOB(const stob::Node* chain){
	Vec2r p = makeVec2rFromSTOB(chain);
	for(unsigned i = 0; i != 2 && chain; ++i, chain = chain->next()){}
	Vec2r d = makeVec2rFromSTOB(chain);
	return Rectr(p, d);
}



morda::Vec2r morda::dimVec2rFromSTOB(const stob::Node* chain){
	unsigned i;
	morda::Vec2r ret;
	
	float v = 0;
	for(i = 0; i != 2 && chain; ++i, chain = chain->next()){
		v = dimValueFromSTOB(*chain);
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}



real morda::dimValueFromSTOB(const stob::Node& n){
	//check if millimeters
	if(n.length() >= 2 && n.value()[n.length() - 1] == 'm' && n.value()[n.length() - 2] == 'm'){
		return App::inst().units.mmToPx(n.asFloat());
	}else if(n.length() >= 2 && n.value()[n.length() - 1] == 't' && n.value()[n.length() - 2] == 'p'){ //check if in points
		return App::inst().units.ptToPx(n.asFloat());
	}
	
	return n.asFloat();
}



std::tuple<std::unique_ptr<stob::Node>, stob::Node*> morda::resolveIncludes(papki::File& fi, std::unique_ptr<stob::Node> begin){
	if(!begin){
		return std::make_tuple(nullptr, nullptr);
	}
	
	const char* DIncludeTag = "include";
	
	auto n = begin->thisOrNext(DIncludeTag);
	for(; n.node();){
		ASSERT(n.node())
		stob::Node* incPathNode = n.node()->child();
		if(!incPathNode){
			throw Exc("include tag without value encountered in resource script");
		}
		TRACE(<< "ResolveIncludes(): incPathNode->Value = " << incPathNode->value() << std::endl)

		fi.setPath(fi.dir() + incPathNode->value());
		std::unique_ptr<stob::Node> incNodes = stob::load(fi);

		//recursive call
		auto ri = resolveIncludes(fi, std::move(incNodes));

		stob::Node* lastChild = std::get<1>(ri);
		
		if(lastChild){
			//substitute includes
			if(!n.prev()){
				//include tag is the very first tag

				ASSERT(!lastChild->next())
				lastChild->insertNext(n.node()->chopNext());
				begin = std::move(std::get<0>(ri));
			}else{
				//include tag is not the first one

				n.prev()->removeNext();

				ASSERT(!lastChild->next())
				std::unique_ptr<stob::Node> tail = n.prev()->chopNext();
				n.prev()->setNext(std::move(std::get<0>(ri)));
				lastChild->setNext(std::move(tail));
			}
			n = lastChild->next(DIncludeTag);
		}else{
			n = n.node()->next(DIncludeTag);
		}
	}
	return std::make_tuple(std::move(begin), n.prev());
}



kolme::Vec2b morda::makeVec2bFromSTOB(const stob::Node* chain){
	unsigned i;
	kolme::Vec2b ret;
	
	bool v = false;
	for(i = 0; i != 2 && chain; ++i, chain = chain->next()){
		v = chain->asBool();
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}



bool morda::isPercent(const stob::Node& n){
	auto len = n.length();
	
	if(len == 0){
		return false;
	}
	
	if(n.value()[len - 1] == '%'){
		return true;
	}
	
	return false;
}
