#include "ResFont.hpp"

#include "../Exc.hpp"
#include "../App.hpp"
#include "../util/util.hpp"

#include <ting/utf8.hpp>

using namespace morda;



ResFont::ResFont(const ting::fs::File& fi, const ting::Buffer<std::uint32_t> chars, unsigned size, unsigned outline) :
		f(fi, chars, size, outline)
{}



//static
std::shared_ptr<ResFont> ResFont::Load(const stob::Node& chain, const ting::fs::File& fi){
	//read chars attribute
	std::vector<std::uint32_t> wideChars;
	for(ting::utf8::Iterator i(chain.side("chars").up().Value()); i.IsNotEnd(); ++i){
		wideChars.push_back(i.Char());
	}

	ASSERT(wideChars.size() > 0)

	//read size attribute
	unsigned size;
	if(auto sizeProp = chain.ChildOfThisOrNext("size")){
		size = unsigned(morda::DimValueFromSTOB(*sizeProp));
	}else{
		size = 13;
	}
	
	//read outline attribute
	unsigned outline;
	if(auto outlineProp = chain.ChildOfThisOrNext("outline")){
		outline = unsigned(outlineProp->AsUint32());
	}else{
		outline = 0;
	}

	fi.SetPath(chain.side("file").up().Value());

	return std::move(ting::New<ResFont>(fi, ting::Buffer<std::uint32_t>(&(*wideChars.begin()), wideChars.size()), size, outline));
}

