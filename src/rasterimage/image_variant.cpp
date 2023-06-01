#include "image_variant.hpp"

#include <png.h>
#include <utki/config.hpp>

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
		const static auto factories_array =
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

namespace {
void png_read_callback(png_structp png_ptr, png_bytep data, png_size_t length)
{
	auto fi = reinterpret_cast<papki::file*>(png_get_io_ptr(png_ptr));
	ASSERT(fi)

	// TODO: get number of butes read and check for EOF, rise error if needed
	fi->read(utki::make_span(data, length));
}
} // namespace

image_variant rasterimage::read_png(papki::file& fi)
{
	ASSERT(!fi.is_open())

	// open file
	papki::file::guard file_guard(fi);

	static const unsigned png_sig_size = 8; // the size of PNG signature

	{
		std::array<png_byte, png_sig_size> sig;
		auto span = utki::make_span(sig);

		std::fill(span.begin(), span.end(), 0);

		auto num_bytes_read = fi.read(span);
		if (num_bytes_read != span.size_bytes()) {
			throw std::invalid_argument("rasterimage::read_png(): could not read file signature");
		}

		// check that it is a PNG file
		if (png_sig_cmp(span.data(), 0, span.size_bytes()) != 0) {
			throw std::invalid_argument("rasterimage::read_png(): not a PNG file");
		}
	}

	// create internal PNG-structure to work with PNG file
	// (no warning and error callbacks)
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	utki::scope_exit png_scope_exit([&png_ptr]() {
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
	});

	// NOTE: the memory freed by png_destroy_read_struct()
	png_infop info_ptr = png_create_info_struct(png_ptr);
	utki::scope_exit info_scope_exit([&png_ptr, &info_ptr] {
		png_destroy_info_struct(png_ptr, &info_ptr);
	});

	png_set_sig_bytes(png_ptr, png_sig_size); // we've already read png_sig_size bytes

	// set custom "ReadFromFile" function
	png_set_read_fn(
		png_ptr,
		const_cast<papki::file*>(&fi), // TODO: why const_cast?
		png_read_callback
	);

	// read in all information about file
	png_read_info(png_ptr, info_ptr);

	// get information from info_ptr
	png_uint_32 width = 0;
	png_uint_32 height = 0;
	int bit_depth = 0;
	int color_format = 0;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_format, nullptr, nullptr, nullptr);

	// we want to convert tRNS transparency (e.g. single color treated as transparent) to proper alpha channel
	png_set_tRNS_to_alpha(png_ptr);

	// convert paletted PNG to rgb image
	if (color_format == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
	}

	// convert grayscale PNG to 8bit greyscale PNG
	if (color_format == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}

#if CFG_ENDIANNESS == CFG_ENDIANNESS_LITTLE
	// PNG stores 16 bit images in network order (big endian),
	// so we ask libpng to convert it to little endian
	png_set_swap(png_ptr);
#endif

	// set gamma information
	double gamma = 0.0f;

	// if there's gamma info in the file, set it to 2.2
	if (png_get_gAMA(png_ptr, info_ptr, &gamma)) {
		png_set_gamma(png_ptr, 2.2, gamma);
	} else {
		png_set_gamma(png_ptr, 2.2, 0.45455); // set to 0.45455 otherwise (good guess for GIF images on PCs)
	}

	// update info after all transformations
	png_read_update_info(png_ptr, info_ptr);

	// get all dimensions and color info again
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_format, nullptr, nullptr, nullptr);

	depth image_depth;

	// strip 16-bit png to 8-bit
	if (bit_depth == 16) {
		image_depth = depth::uint_16_bit;
	} else {
		image_depth = depth::uint_8_bit;
	}

	// set image type
	format image_format;
	switch (color_format) {
		case PNG_COLOR_TYPE_GRAY:
			image_format = format::grey;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			image_format = format::greya;
			break;
		case PNG_COLOR_TYPE_RGB:
			image_format = format::rgb;
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			image_format = format::rgba;
			break;
		default:
			throw std::invalid_argument("rasterimage::read_png(): unknown color_format");
	}

	image_variant im({width, height}, image_format, image_depth);

	// get PNG bytes per row
	png_size_t num_bytes_per_row = png_get_rowbytes(png_ptr, info_ptr);

	// check that our expectations are correct
	if (num_bytes_per_row != png_size_t(im.dims().x()) * png_size_t(im.num_channels())) {
		throw std::runtime_error("rasterimage::read_png(): number of bytes per row does not match expected value");
	}

	std::visit(
		[&](auto&& image) {
			using image_type = std::remove_reference_t<decltype(image)>;
			using depth_type = typename image_type::pixel_type::value_type;
			if constexpr ( //
				!std::is_same_v<depth_type, uint8_t> && //
				!std::is_same_v<depth_type, uint16_t> //
			)
			{
				// only 8 or 16 bit images are supported
				throw std::invalid_argument("rasterimage::read_png(): unsupported image depth");
			} else {
				ASSERT((num_bytes_per_row * height) == image.pixels().size_bytes())
				ASSERT(image.dims().y() != 0 && image.pixels().size() != 0)

				// make an array of row pointers
				std::vector<png_bytep> rows(image.dims().y());
				std::generate( //
					rows.begin(),
					rows.end(),
					[&image, i = image.begin()]() mutable {
						ASSERT(i != image.end())
						auto ret = i->data();
						++i;
						return reinterpret_cast<decltype(rows)::value_type>(ret);
					}
				);

				// read in image data
				png_read_image(png_ptr, rows.data());
			}
		},
		im.variant
	);

	return im;
}
