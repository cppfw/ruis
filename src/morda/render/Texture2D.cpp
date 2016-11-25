#include "Texture2D.hpp"

using namespace morda;

unsigned Texture2D::bytesPerPixel(Texture2D::TexType_e t) {
	switch(t){
		case Texture2D::TexType_e::GREY:
			return 1;
		case Texture2D::TexType_e::GREYA:
			return 2;
		case Texture2D::TexType_e::RGB:
			return 3;
		case Texture2D::TexType_e::RGBA:
			return 4;
		default:
			return 0;
	}
}
