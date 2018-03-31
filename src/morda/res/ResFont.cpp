#include "ResFont.hpp"

#include "../Exc.hpp"

#include "../Morda.hpp"

#include "../util/util.hpp"

#include "../fonts/TexFont.hxx"

#include <unikod/utf8.hpp>

using namespace morda;



ResFont::ResFont(const papki::File& fi, const std::u32string& chars, unsigned fontSize) :
		f(utki::makeUnique<TexFont>(fi, chars, fontSize))
{}



std::shared_ptr<ResFont> ResFont::load(const stob::Node& chain, const papki::File& fi){
	//read chars attribute
	auto wideChars = unikod::toUtf32(chain.side("chars").up().value());

	ASSERT(wideChars.size() > 0)

	//read size attribute
	unsigned fontSize;
	if(auto sizeProp = chain.childOfThisOrNext("size")){
		fontSize = unsigned(morda::dimValueFromSTOB(*sizeProp));
	}else{
		fontSize = 13;
	}

	fi.setPath(chain.side("file").up().value());

	return std::make_shared<ResFont>(fi, wideChars, fontSize);
}

