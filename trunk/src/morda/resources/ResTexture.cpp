#include "ResTexture.hpp"

#include "../Exc.hpp"
#include "../util/Image.hpp"



using namespace morda;



//static
std::shared_ptr<ResTexture> ResTexture::Load(const stob::Node& el, ting::fs::File& fi){
//	TRACE(<< "ResTexture::Load(): enter" << std::endl)
	const stob::Node* fileVal = el.GetProperty("file");
	if(!fileVal){
		throw morda::Exc("ResTexture::Load(): no 'file' property in resource description");
	}

//	TRACE(<< "ResTexture::Load(): Loading image, file path = " << fileVal->Value() << std::endl)
	fi.SetPath(fileVal->Value());
	Image image(fi);
//	TRACE(<< "ResTexture::Load(): image loaded" << std::endl)
	image.FlipVertical();

	return std::move(ting::New<ResTexture>(image));
}
