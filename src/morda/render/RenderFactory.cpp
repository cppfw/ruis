#include "RenderFactory.hpp"

using namespace morda;

std::shared_ptr<Texture2D> RenderFactory::createTexture2D(r4::vec2ui dim, const utki::span<std::uint32_t>& data) {
	return this->createTexture2D(
			Texture2D::TexType_e::RGBA,
			dim,
			utki::span<std::uint8_t>(
					const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(&*data.begin())),
					data.sizeInBytes()
				)
		);
}
