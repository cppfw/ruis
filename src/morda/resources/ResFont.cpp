#include "ResFont.hpp"

#include "../Exc.hpp"
#include "../App.hpp"
#include "../util/util.hpp"

#include <ting/utf8.hpp>

using namespace morda;



ResFont::ResFont(ting::fs::File& fi, const ting::Buffer<ting::u32>& chars, unsigned size, unsigned outline) :
		f(fi, chars, size, outline)
{}



//static
ting::Ref<ResFont> ResFont::Load(const stob::Node& el, ting::fs::File& fi){
	//get font filename
	const stob::Node* fileProp = el.GetProperty("file");
	if(!fileProp){
		throw morda::Exc("ResFont::Load(): no 'file' property in resource description");
	}

	//read chars attribute
	std::vector<ting::u32> wideChars;
	{
		const stob::Node* charsProp = el.GetProperty("chars");
		if(!charsProp){
			throw morda::Exc("ResFont::Load(): no 'chars' property in resource description");
		}
		
		for(ting::utf8::Iterator i(charsProp->Value()); i.IsNotEnd(); ++i){
			wideChars.push_back(i.Char());
		}
	}
	ASSERT(wideChars.size() > 0)

	//read size attribute
	unsigned size;
	if(const stob::Node* sizeProp = el.GetProperty("size")){
		size = unsigned(morda::DimValue(*sizeProp));
	}else{
		size = 13;
	}
	
	//read outline attribute
	unsigned outline;
	{
		const stob::Node* outlineProp = el.GetProperty("outline");
		if(!outlineProp){
			outline = 0;
		}else{
			outline = unsigned(outlineProp->AsU32());
		}
	}

	fi.SetPath(fileProp->Value());

	return ting::New<ResFont>(fi, ting::Buffer<ting::u32>(&(*wideChars.begin()), wideChars.size()), size, outline);
}

