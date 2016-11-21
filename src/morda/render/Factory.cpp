#include "Factory.hpp"

using namespace morda;



std::shared_ptr<Texture2D_n> Factory::createTexture2D(kolme::Vec2ui dim, const utki::Buf<std::uint32_t>& data) {
	return this->createTexture2D(
			Texture2D_n::TexType_e::RGBA,
			dim,
			utki::Buf<std::uint8_t>(
					const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(&*data.begin())),
					data.sizeInBytes()
				)
		);
}

