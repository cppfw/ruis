#include "ResTexture.hpp"

#include "../Exc.hpp"
#include "../util/Image.hpp"
#include "../util/util.hpp"



using namespace morda;



//static
std::shared_ptr<ResTexture> ResTexture::Load(const stob::Node& chain, const ting::fs::File& fi){
//	TRACE(<< "ResTexture::Load(): enter" << std::endl)

//	TRACE(<< "ResTexture::Load(): Loading image, file path = " << fileVal->Value() << std::endl)
	fi.SetPath(chain.side("file").up().Value());
	Image image(fi);
//	TRACE(<< "ResTexture::Load(): image loaded" << std::endl)
	image.FlipVertical();

	return std::move(ting::New<ResTexture>(image));
}
