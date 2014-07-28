#include "util.hpp"

#include "../App.hpp"

#include <ting/math.hpp>
#include <ting/debug.hpp>


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



morda::Vec2f morda::DimVec2f(const stob::Node* chain){
	unsigned i;
	morda::Vec2f ret;
	
	float v = 0;
	for(i = 0; i != 2 && chain; ++i, chain = chain->Next()){
		v = DimValue(*chain);
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}



float morda::DimValue(const stob::Node& n){
	//check if millimeters
	if(n.ValueLength() >= 2 && n.Value()[n.ValueLength() - 1] == 'm' && n.Value()[n.ValueLength() - 2] == 'm'){
		float ret = ting::math::Round(n.AsFloat() * App::Inst().DotsPerCm() / 10.0f);
//		TRACE_ALWAYS(<< "ParseDistanceValue(): mm, ret = " << ret << std::endl)
		return ret;
	}
	
	return n.AsFloat();
}



std::tuple<std::unique_ptr<stob::Node>, stob::Node*> morda::ResolveIncludes(ting::fs::File& fi, std::unique_ptr<stob::Node> begin){
	if(!begin){
		return std::make_tuple(std::unique_ptr<stob::Node>(), nullptr);
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

		fi.SetPath(fi.ExtractDirectory() + incPathNode->Value());
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

