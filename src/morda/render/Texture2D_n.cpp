#include "Texture2D_n.hpp"

using namespace morda;

unsigned Texture2D_n::bytesPerPixel(Texture2D_n::TexType_e t) {
	switch(t){
		case Texture2D_n::TexType_e::GREY:
			return 1;
		case Texture2D_n::TexType_e::GREYA:
			return 2;
		case Texture2D_n::TexType_e::RGB:
			return 3;
		case Texture2D_n::TexType_e::RGBA:
			return 4;
		default:
			return 0;
	}
}
