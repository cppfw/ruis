#pragma once

#include <variant>

#include "image.hpp"

namespace rasterimage {

enum class depth {
	uint_8_bit,
	uint_16_bit,
	float_32_bit,

	enum_size
};

template <depth depth_enum>
using depth_type_t = std::conditional_t<
	depth_enum == depth::uint_8_bit,
	uint8_t,
	std::conditional_t<
		depth_enum == depth::uint_16_bit,
		uint16_t,
		std::conditional_t<depth_enum == depth::float_32_bit, float, void>>>;

enum class format {
	grey,
	greya,
	rgb,
	rgba,

	enum_size
};

inline constexpr size_t to_num_channels(format f)
{
	return size_t(f) + 1;
}

// TODO: doxygen
class image_variant
{
public:
	using variant_type = std::variant<
		image<uint8_t, 1>,
		image<uint8_t, 2>,
		image<uint8_t, 3>,
		image<uint8_t, 4>,
		image<uint16_t, 1>,
		image<uint16_t, 2>,
		image<uint16_t, 3>,
		image<uint16_t, 4>,
		image<float, 1>,
		image<float, 2>,
		image<float, 3>,
		image<float, 4>>;

private:
	variant_type variant;

	static size_t to_variant_index(format pixel_format, depth channel_depth);

public:
	image_variant(
		const r4::vector2<uint32_t>& dimensions = {0, 0},
		format pixel_format = format::rgba,
		depth channel_depth = depth::uint_8_bit
	);

	size_t num_channels() const noexcept
	{
		auto ret = size_t(this->get_format()) + 1;
		ASSERT(
			std::visit(
				[](const auto& sfi) {
					return sfi.num_channels;
				},
				this->variant
			)
			== ret
		)
		return ret;
	}

	format get_format() const noexcept
	{
		auto ret = format(this->variant.index() % size_t(format::enum_size));
		ASSERT(
			format(std::visit(
				[](const auto& sfi) {
					return sfi.num_channels - 1;
				},
				this->variant
			))
			== ret
		)
		return ret;
	}

	depth get_depth() const noexcept
	{
		return depth(this->variant.index() / size_t(format::enum_size));
	}

	const dimensioned::dimensions_type& dims() const noexcept;

	template <format components_enum, depth depth_enum = depth::uint_8_bit>
	image<depth_type_t<depth_enum>, to_num_channels(components_enum)>& get()
	{
		return std::get<image<depth_type_t<depth_enum>, to_num_channels(components_enum)>>(this->variant);
	}

	template <format components_enum, depth depth_enum = depth::uint_8_bit>
	const image<depth_type_t<depth_enum>, to_num_channels(components_enum)>& get() const
	{
		return std::get<image<depth_type_t<depth_enum>, to_num_channels(components_enum)>>(this->variant);
	}
};

} // namespace rasterimage
