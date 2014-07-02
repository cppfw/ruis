#include "ResFont.hpp"

#include "../Exc.hpp"
#include "../App.hpp"


using namespace morda;



ResFont::ResFont(ting::fs::File& fi, const ting::Buffer<ting::u32>& chars, unsigned size, unsigned outline) :
		f(App::Inst().Shaders().simpleTexturing, fi, chars, size, outline)
{}



//static
ting::Ref<ResFont> ResFont::Load(const stob::Node* el, ting::fs::File& fi){
	ASSERT(el)
	//get font filename
	const stob::Node* fileProp = el->GetProperty("file");
	if(!fileProp){
		throw morda::Exc("ResFont::Load(): no 'file' property in resource description");
	}

	//read chars attribute
	std::vector<ting::u32> wideChars;
	{
		const stob::Node* charsProp = el->GetProperty("chars");
		if(!charsProp){
			throw morda::Exc("ResFont::Load(): no 'chars' property in resource description");
		}
		
		//TODO: do utf8 to utf32
		const std::string& charsStr = charsProp->Value();
		for(unsigned i = 0; i < charsStr.size(); ++i){
			wideChars.push_back(charsStr[i]);
		}
	}
	ASSERT(wideChars.size() > 0)

	//read size attribute
	unsigned size;
	{
		const stob::Node* sizeProp = el->GetProperty("size");
		if(!sizeProp){
			size = 12;
		}else{
			size = unsigned(sizeProp->AsU32());
		}
	}
	
	//read outline attribute
	unsigned outline;
	{
		const stob::Node* outlineProp = el->GetProperty("outline");
		if(!outlineProp){
			outline = 0;
		}else{
			outline = unsigned(outlineProp->AsU32());
		}
	}

	fi.SetPath(fileProp->Value());

	return ting::Ref<ResFont>(new ResFont(fi, ting::Buffer<ting::u32>(&(*wideChars.begin()), wideChars.size()), size, outline));
}

