#include "image_variant.hpp"

using namespace rasterimage;

using factory_type = std::add_pointer_t<image_variant::variant_type(const r4::vector2<uint32_t>& dimensions)>;

// creates std::array of factory functions which construct image_variant::variant_type
// initialized to alternative index same as factory's index in the array
template <size_t... index>
std::array<factory_type, sizeof...(index)> make_factories_array(std::index_sequence<index...>)
{
	return {[](const r4::vector2<uint32_t>& dimensions) {
		return image_variant::variant_type(std::in_place_index<index>, dimensions);
	}...};
}

size_t image_variant::to_variant_index(format pixel_format, depth channel_depth)
{
	auto ret = size_t(channel_depth) * size_t(format::enum_size) + size_t(pixel_format);
	ASSERT(ret < std::variant_size_v<image_variant::variant_type>)
	return ret;
}

image_variant::image_variant(const r4::vector2<uint32_t>& dimensions, format pixel_format, depth channel_depth) :
	variant([&]() {
		const auto factories_array =
			make_factories_array(std::make_index_sequence<std::variant_size_v<image_variant::variant_type>>());

		auto i = to_variant_index(pixel_format, channel_depth);

		return factories_array[i](dimensions);
	}())
{}

const dimensioned::dimensions_type& image_variant::dims() const noexcept
{
	try {
		ASSERT(!this->variant.valueless_by_exception())
		return std::visit(
			[&](const dimensioned& sfi) -> const auto& {
				return sfi.dims();
			},
			this->variant
		);
	} catch (std::bad_variant_access& e) {
		// this->variant must never be valueless_by_exeception,
		// so should never reach here
		ASSERT(false)
		abort();
	}
}
