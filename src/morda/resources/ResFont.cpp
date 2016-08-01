#include "ResFont.hpp"

#include "../Exc.hpp"

#include "../Morda.hpp"

#include "../util/util.hpp"

#include <unikod/utf8.hpp>

using namespace morda;



ResFont::ResFont(const papki::File& fi, const utki::Buf<std::uint32_t> chars, unsigned size, unsigned outline) :
		f(fi, chars, size, outline)
{}



//static
std::shared_ptr<ResFont> ResFont::load(const stob::Node& chain, const papki::File& fi){
	//read chars attribute
	std::vector<std::uint32_t> wideChars;
	for(unikod::Utf8Iterator i(chain.side("chars").up().value()); !i.isEnd(); ++i){
		wideChars.push_back(i.character());
	}

	ASSERT(wideChars.size() > 0)

	//read size attribute
	unsigned size;
	if(auto sizeProp = chain.childOfThisOrNext("size")){
		size = unsigned(morda::dimValueFromSTOB(*sizeProp));
	}else{
		size = 13;
	}
	
	//read outline attribute
	unsigned outline;
	if(auto outlineProp = chain.childOfThisOrNext("outline")){
		outline = unsigned(outlineProp->asUint32());
	}else{
		outline = 0;
	}

	fi.setPath(chain.side("file").up().value());

	return utki::makeShared<ResFont>(fi, utki::Buf<std::uint32_t>(&(*wideChars.begin()), wideChars.size()), size, outline);
}

