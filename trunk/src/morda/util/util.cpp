#include "util.hpp"

#include "../App.hpp"

#include <ting/math.hpp>
#include <ting/debug.hpp>


using namespace morda;



morda::Vec2r morda::Vec2rFromSTOB(const stob::Node* chain){
	unsigned i;
	morda::Vec2r ret;
	
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


Rect2r morda::Rect2rFromSTOB(const stob::Node* chain){
	Vec2r p = Vec2rFromSTOB(chain);
	for(unsigned i = 0; i != 2 && chain; ++i, chain = chain->Next()){}
	Vec2r d = Vec2rFromSTOB(chain);
	return Rect2r(p, d);
}



morda::Vec2r morda::DimVec2rFromSTOB(const stob::Node* chain){
	unsigned i;
	morda::Vec2r ret;
	
	float v = 0;
	for(i = 0; i != 2 && chain; ++i, chain = chain->Next()){
		v = DimValueFromSTOB(*chain);
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}



real morda::DimValueFromSTOB(const stob::Node& n){
	//check if millimeters
	if(n.ValueLength() >= 2 && n.Value()[n.ValueLength() - 1] == 'm' && n.Value()[n.ValueLength() - 2] == 'm'){
		real ret = ting::math::Round(n.AsFloat() * App::Inst().DotsPerCm() / 10.0f);
//		TRACE_ALWAYS(<< "ParseDistanceValue(): mm, ret = " << ret << std::endl)
		return ret;
	}
	
	return n.AsFloat();
}



std::tuple<std::unique_ptr<stob::Node>, stob::Node*> morda::ResolveIncludes(ting::fs::File& fi, std::unique_ptr<stob::Node> begin){
	if(!begin){
		return std::make_tuple(nullptr, nullptr);
	}
	
	const char* DIncludeTag = "include";
	
	stob::Node::NodeAndPrev n = begin->ThisOrNext(DIncludeTag);
	for(; n.node();){
		ASSERT(n.node())
		stob::Node* incPathNode = n.node()->Child();
		if(!incPathNode){
			throw Exc("include tag without value encountered in resource script");
		}
		TRACE(<< "ResolveIncludes(): incPathNode->Value = " << incPathNode->Value() << std::endl)

		fi.SetPath(fi.Dir() + incPathNode->Value());
		std::unique_ptr<stob::Node> incNodes = stob::Load(fi);

		//recursive call
		auto ri = ResolveIncludes(fi, std::move(incNodes));

		stob::Node* lastChild = std::get<1>(ri);
		
		if(lastChild){
			//substitute includes
			if(!n.prev()){
				//include tag is the very first tag

				ASSERT(!lastChild->Next())
				lastChild->InsertNext(std::move(n.node()->ChopNext()));
				begin = std::move(std::get<0>(ri));
			}else{
				//include tag is not the first one

				n.prev()->RemoveNext();

				ASSERT(!lastChild->Next())
				std::unique_ptr<stob::Node> tail = n.prev()->ChopNext();
				n.prev()->SetNext(std::move(std::get<0>(ri)));
				lastChild->SetNext(std::move(tail));
			}
			n = lastChild->Next(DIncludeTag);
		}else{
			n = n.node()->Next(DIncludeTag);
		}
	}
	return std::make_tuple(std::move(begin), n.prev());
}



morda::Vector2<bool> morda::Vec2bFromSTOB(const stob::Node* chain){
	unsigned i;
	morda::Vector2<bool> ret;
	
	bool v = false;
	for(i = 0; i != 2 && chain; ++i, chain = chain->Next()){
		v = chain->AsBool();
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}
