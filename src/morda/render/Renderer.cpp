#include "Renderer.hpp"

using namespace morda;



unsigned Renderer::bytesPerPixel(TexType_e t) {
	switch(t){
		case TexType_e::GREY:
			return 1;
		case TexType_e::GREYA:
			return 2;
		case TexType_e::RGB:
			return 3;
		case TexType_e::RGBA:
			return 4;
		default:
			return 0;
	}
}
