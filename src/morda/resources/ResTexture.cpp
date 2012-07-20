#include "ResTexture.hpp"

#include "../Exc.hpp"
#include "../util/Image.hpp"



using namespace morda;



//static
ting::Ref<ResTexture> ResTexture::Load(const stob::Node* el, ting::fs::File& fi){
	const stob::Node* fileProp = el->Child("file").second;
	if(!fileProp){
		throw morda::Exc("ResTexture::Load(): no 'file' property in resource description");
	}
	const stob::Node* fileVal = fileProp->Child();
	if(!fileVal){
		throw morda::Exc("ResTexture::Load(): no value assigned to 'file' property in resource description");
	}

//	M_RESMANIMPL_LOG(<< "ResTexture::Load(): fileName = " << fileName << std::endl)

	fi.SetPath(fileVal->Value());
	Image image(fi);
	image.FlipVertical();

	return ting::Ref<ResTexture>(new ResTexture(image));
}
