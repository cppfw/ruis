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



stob::Node* morda::ResolveIncludes(ting::fs::File& fi, stob::Node& root){
	const char* DIncludeTag = "include";
	
	stob::Node::NodeAndPrev n = root.Child(DIncludeTag);
	for(; n.node();){
		ASSERT(n.node())
		stob::Node* incPathNode = n.node()->Child();
		if(!incPathNode){
			throw Exc("include tag without value encountered in resource script");
		}
		TRACE(<< "ResolveIncludes(): incPathNode->Value = " << incPathNode->Value() << std::endl)

		fi.SetPath(fi.ExtractDirectory() + incPathNode->Value());
		ting::Ptr<stob::Node> incNode = stob::Node::New();
		incNode->SetChildren(stob::Load(fi));

		//recursive call
		stob::Node* lastChild = ResolveIncludes(fi, *incNode);

		//substitute includes
		if(!n.prev()){
			//include tag is the very first tag
			root.RemoveFirstChild();

			if(lastChild){
				ASSERT(!lastChild->Next())
				ASSERT(incNode->Child())
				lastChild->InsertNext(root.RemoveChildren());
				root.SetChildren(incNode->RemoveChildren());
				n = lastChild->Next(DIncludeTag);
			}else{
				ASSERT(!incNode->Child())
				n = n.node()->Next(DIncludeTag);
			}
			continue;
		}else{
			//include tag is not the first one

			n.prev()->RemoveNext();
			if(lastChild){
				ASSERT(!lastChild->Next())
				ASSERT(incNode->Child())
				ting::Ptr<stob::Node> tail = n.prev()->ChopNext();
				n.prev()->SetNext(incNode->RemoveChildren());
				lastChild->SetNext(tail);
				n = lastChild->Next(DIncludeTag);
			}else{
				ASSERT(!incNode->Child())
				n = n.node()->Next(DIncludeTag);
			}
			continue;
		}
	}
	return n.prev();
}

