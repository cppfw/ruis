#include "render_factory.hpp"

using namespace morda;

std::shared_ptr<Texture2D> render_factory::create_texture_2d(r4::vec2ui dims, const utki::span<uint32_t> data){
	return this->create_texture_2d(
			Texture2D::TexType_e::RGBA,
			dims,
			utki::span<std::uint8_t>(
					const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(&*data.begin())),
					data.size_bytes()
				)
		);
}
