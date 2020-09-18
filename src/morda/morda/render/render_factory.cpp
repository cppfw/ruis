#include "render_factory.hpp"

using namespace morda;

std::shared_ptr<texture_2d> render_factory::create_texture_2d(r4::vector2<unsigned> dims, utki::span<const uint32_t> data){
	return this->create_texture_2d(
			texture_2d::type::rgba,
			dims,
			utki::span<std::uint8_t>(
					const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(&*data.begin())),
					data.size_bytes()
				)
		);
}
