#include "ResFont.hpp"

#include "../Exc.hpp"


using namespace morda;


//static
ting::Ref<ResFont> ResFont::Load(const stob::Node* el, ting::fs::File& fi){
	ASSERT(el)
	//get font filename
	const stob::Node* fileProp = el->Child("file").second;
	if(!fileProp){
		throw morda::Exc("ResFont::Load(): no 'file' property found");
	}
	const stob::Node* fileVal = fileProp->Child();
	if(!fileVal){
		throw morda::Exc("ResFont::Load(): 'file' property has no value");
	}

	//read chars attribute
	std::wstring wideChars;
	{
		const stob::Node* charsProp = el->Child("chars").second;
		if(!charsProp){
			throw morda::Exc("ResFont::Load(): no 'chars' property found");
		}
		const stob::Node* charsVal = charsProp->Child();
		if(!charsVal){
			throw morda::Exc("ResFont::Load(): 'chars' property has no value");
		}
		
		//TODO: do utf8 to utf32
		const std::string& charsStr = charsVal->Value();
		for(unsigned i = 0; i < charsStr.size(); ++i){
			wideChars += charsStr[i];
		}
	}
	ASSERT(wideChars.size() > 0)

	//read size attribute
	unsigned size;
	{
		const stob::Node* sizeProp = el->Child("size").second;
		if(!sizeProp){
			size = 10;
		}else{
			const stob::Node* sizeVal = sizeProp->Child();
			if(!sizeVal){
				size = 10;
			}else{
				size = unsigned(sizeVal->AsU32());
			}
		}
	}
	
	//read outline attribute
	unsigned outline;
	{
		const stob::Node* outlineProp = el->Child("outline").second;
		if(!outlineProp){
			outline = 0;
		}else{
			const stob::Node* outlineVal = outlineProp->Child();
			if(!outlineVal){
				outline = 0;
			}else{
				outline = unsigned(outlineVal->AsU32());
			}
		}
	}

	fi.SetPath(fileVal->Value());

	return ting::Ref<ResFont>(new ResFont(fi, wideChars, size, outline));
}

