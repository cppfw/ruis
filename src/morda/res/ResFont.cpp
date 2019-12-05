#include "ResFont.hpp"

#include "../exception.hpp"

#include "../Morda.hpp"

#include "../util/util.hpp"

#include "../fonts/TexFont.hxx"

#include <unikod/utf8.hpp>

using namespace morda;



ResFont::ResFont(const papki::File& fi, unsigned fontSize, unsigned maxCached) :
		f(utki::makeUnique<TexFont>(fi, fontSize, maxCached))
{}



std::shared_ptr<ResFont> ResFont::load(const stob::Node& chain, const papki::File& fi){
	//read size attribute
	unsigned fontSize;
	if(auto sizeProp = chain.childOfThisOrNext("size")){
		fontSize = unsigned(morda::dimValueFromSTOB(*sizeProp));
	}else{
		fontSize = 13;
	}

	fi.setPath(chain.side("file").up().value());

	unsigned maxCached = unsigned(-1);
	if(auto p = chain.thisOrNext("maxCached").get_node()){
		maxCached = p->up().asUint32();
	}

	return std::make_shared<ResFont>(fi, fontSize, maxCached);
}

