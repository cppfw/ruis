#include "Factory.hpp"

using namespace morda;



unsigned Factory::bytesPerPixel(TexType_e t) {
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

std::shared_ptr<Texture2D_n> Factory::createTexture2D(kolme::Vec2ui dim, const utki::Buf<std::uint32_t>& data) {
	return this->createTexture2D(
			TexType_e::RGBA,
			dim,
			utki::Buf<std::uint8_t>(
					const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(&*data.begin())),
					data.sizeInBytes()
				)
		);
}

