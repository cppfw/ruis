/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include <algorithm>
#include <cstring>

#include <png.h>

// JPEG lib does not have 'extern "C"{}' :-(, so we put it outside of their .h
// or will have linking problems otherwise because
// of "_" symbol in front of C-function names
extern "C" {
#include <jpeglib.h>
}

#include "raster_image.hpp"

using namespace morda;

void raster_image::init(r4::vector2<unsigned> dimensions, color_depth pixel_color_depth)
{
	this->reset();
	this->dims_ = dimensions;
	this->color_depth_ = pixel_color_depth;
	this->buffer.resize(size_t(this->dims().x() * this->dims().y()) * this->num_channels());
}

raster_image::raster_image(r4::vector2<unsigned> dimensions, color_depth pixel_color_depth, const uint8_t* src_buf)
{
	ASSERT(src_buf)
	this->init(dimensions, pixel_color_depth);

	std::copy(src_buf, src_buf + this->buffer.size(), this->buffer.begin());
}

raster_image::raster_image(r4::vector2<unsigned> pos, r4::vector2<unsigned> dimensions, const raster_image& src)
{
	if (src.dims().x() == 0 || src.dims().y() == 0) {
		throw std::invalid_argument("Image::Image(): source image has zero dimensions");
	}

	if (src.dims().x() <= pos.x() || src.dims().y() <= pos.y() || src.dims().x() < (pos.x() + dimensions.x())
		|| src.dims().y() < (pos.y() + dimensions.y()))
	{
		throw std::invalid_argument("Image::Image(): incorrect dimensions of given images");
	}

	this->init(dimensions, src.depth());

	// copy image data
	throw std::runtime_error(
		"Image::Image(unsigned x, unsigned y, unsigned width, unsigned height, const Image& src): is not implemented"
	);
	// for(unsigned j=0; j<this->H(); ++j)
	//     memcpy(data+j*w*channels,src->data+((j+y)*src->w+x)*channels,w*channels);
}

// fills image buffer with zeroes
void raster_image::clear(uint8_t val)
{
	if (this->buffer.size() == 0) {
		return;
	}

	std::fill(this->buffer.begin(), this->buffer.end(), val);
}

void raster_image::clear(unsigned chan, uint8_t val)
{
	for (unsigned i = 0; i < this->dims().x() * this->dims().y(); ++i) {
		this->buffer[i * this->num_channels() + chan] = val;
	}
}

void raster_image::reset()
{
	this->dims_.set(0);
	this->color_depth_ = color_depth::unknown;
	this->buffer.clear();
}

void raster_image::flip_vertical()
{
	if (!this->buffer.size()) {
		// nothing to flip
		return;
	}

	unsigned stride = this->num_channels() * this->dims().x(); // stride
	std::vector<uint8_t> line(stride);

	for (auto t = this->buffer.begin(), b = utki::prev(this->buffer.end(), stride);
		 t != utki::next(this->buffer.begin(), size_t(stride) * (this->dims().y() / 2));
		 t = utki::next(t, stride), b = utki::prev(b, stride))
	{
		// move line to temp
		std::copy(t, std::next(t, stride), line.begin());

		// move bottom line to top
		std::copy(b, std::next(b, stride), t);

		// move line from temp
		std::copy(line.begin(), line.end(), b);
	}
}

void raster_image::blit(r4::vector2<unsigned> pos, const raster_image& src)
{
	ASSERT(this->buffer.size() != 0)
	if (this->depth() != src.depth()) {
		throw std::invalid_argument("Image::Blit(): bits per pixel values do not match");
	}

	using std::min;

	auto blit_area = min(src.dims(), this->dims() - pos);

	// TODO: implement blitting for all image types
	switch (this->depth()) {
		case color_depth::grey:
			for (unsigned j = 0; j < blit_area.y(); ++j) {
				for (unsigned i = 0; i < blit_area.x(); ++i) {
					this->pix_chan(i + pos.x(), j + pos.y(), 0) = src.pix_chan(i, j, 0);
				}
			}
			break;
		case color_depth::grey_alpha:
			for (unsigned j = 0; j < blit_area.y(); ++j) {
				for (unsigned i = 0; i < blit_area.x(); ++i) {
					this->pix_chan(i + pos.x(), j + pos.y(), 0) = src.pix_chan(i, j, 0);
					this->pix_chan(i + pos.x(), j + pos.y(), 1) = src.pix_chan(i, j, 1);
				}
			}
			break;
		default:
			ASSERT(false, [&](auto& o) {
				o << "Image::Blit(): unknown image type";
			})
			break;
	}
}

void raster_image::blit(r4::vector2<unsigned> pos, const raster_image& src, unsigned dst_chan, unsigned src_chan)
{
	ASSERT(this->buffer.size())
	if (dst_chan >= this->num_channels()) {
		throw std::invalid_argument(
			"Image::Blit(): destination channel index is greater than number of channels in the image"
		);
	}

	if (src_chan >= src.num_channels()) {
		throw std::invalid_argument(
			"Image::Blit(): source channel index is greater than number of channels in the image"
		);
	}

	using std::min;

	auto blit_area = min(src.dims(), this->dims() - pos);

	for (unsigned j = 0; j < blit_area.y(); ++j) {
		for (unsigned i = 0; i < blit_area.x(); ++i) {
			this->pix_chan(i + pos.x(), j + pos.y(), dst_chan) = src.pix_chan(i, j, src_chan);
		}
	}
}

// custom file read function for PNG
namespace {
void png_callback_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
	auto fi = reinterpret_cast<papki::file*>(png_get_io_ptr(png_ptr));
	ASSERT(fi)
	//	TRACE(<< "png_callback_read: fi = " << fi << " png_ptr = " << png_ptr << " data = " << std::hex << data << "
	// length = " << length << std::endl)
	try {
		auto buf_wrapper = utki::make_span(data, length);
		fi->read(buf_wrapper);
		//		TRACE(<< "png_callback_read: fi->Read() finished" << std::endl)
	} catch (...) {
		// do not let any exception get out of this function
		//		TRACE(<< "png_callback_read: fi->Read() failed" << std::endl)
	}
}
} // namespace

// read PNG file
void raster_image::load_png(const papki::file& fi)
{
	ASSERT(!fi.is_open())

	if (this->buffer.size() > 0) {
		this->reset();
	}

	papki::file::guard file_guard(fi);
	//	TRACE(<< "Image::LoadPNG(): file opened" << std::endl)

	const unsigned png_sig_size = 8; // the size of PNG signature (max 8 bytes)

	{
		std::array<png_byte, png_sig_size> sig;
		auto span = utki::make_span(sig);

		std::fill(span.begin(), span.end(), 0);

		auto num_bytes_read = fi.read(span);
		if (num_bytes_read != span.size_bytes()) {
			throw std::invalid_argument("raster_image::load_png(): could not read file signature");
		}

		// check that it is a PNG file
		if (png_sig_cmp(span.data(), 0, span.size_bytes()) != 0) {
			throw std::invalid_argument("raster_image::load_png(): not a PNG file");
		}
	}

	//	TRACE(<< "Image::LoadPNG(): file is a PNG" << std::endl)

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
	png_set_read_fn(png_ptr, const_cast<papki::file*>(&fi), png_callback_read);

	png_read_info(png_ptr, info_ptr); // read in all information about file

	// get information from info_ptr
	png_uint_32 width = 0;
	png_uint_32 height = 0;
	int bit_depth = 0;
	int color_type = 0;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, nullptr, nullptr, nullptr);

	// strip 16-bit png to 8-bit
	if (bit_depth == 16) {
		png_set_strip_16(png_ptr);
	}
	// convert paletted PNG to rgb image
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
	}
	// convert grayscale PNG to 8bit greyscale PNG
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	}

	// TODO: is this needed? remove?
	// if(png_get_valid(png_ptr, info_ptr,PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

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
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, nullptr, nullptr, nullptr);
	ASSERT(bit_depth == 8)

	// set image type
	raster_image::color_depth image_color_depth;
	switch (color_type) {
		case PNG_COLOR_TYPE_GRAY:
			image_color_depth = raster_image::color_depth::grey;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			image_color_depth = raster_image::color_depth::grey_alpha;
			break;
		case PNG_COLOR_TYPE_RGB:
			image_color_depth = raster_image::color_depth::rgb;
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			image_color_depth = raster_image::color_depth::rgba;
			break;
		default:
			throw std::runtime_error("Image::LoadPNG(): unknown color_type");
	}

	// set image dimensions and set buffer size
	this->init(r4::vector2<unsigned>(width, height), image_color_depth); // set buf array size (allocate memory)

	//	TRACE(<< "Image::LoadPNG(): memory for image allocated" << std::endl)

	// read image data
	png_size_t num_bytes_per_row = png_get_rowbytes(png_ptr, info_ptr); // get bytes per row

	// check that our expectations are correct
	if (num_bytes_per_row != png_size_t(this->dims().x()) * png_size_t(this->num_channels())) {
		throw std::runtime_error("Image::LoadPNG(): number of bytes per row does not match expected value");
	}

	ASSERT((num_bytes_per_row * height) == this->buffer.size())

	//	TRACE(<< "Image::LoadPNG(): going to read in the data" << std::endl)
	{
		ASSERT(this->dims().y() && this->buffer.size())
		std::vector<png_bytep> rows(this->dims().y());
		// initialize row pointers
		//		M_IMAGE_PRINT(<< "Image::LoadPNG(): this->buf.Buf() = " << std::hex << this->buf.Buf() << std::endl)
		for (unsigned i = 0; i < this->dims().y(); ++i) {
			rows[i] = &*this->buffer.begin() + i * num_bytes_per_row;
			//			M_IMAGE_PRINT(<< "Image::LoadPNG(): rows[i] = " << std::hex << rows[i] << std::endl)
		}
		//		TRACE(<< "Image::LoadPNG(): row pointers are set" << std::endl)
		// read in image data
		png_read_image(png_ptr, &*rows.begin());
		//		TRACE(<< "Image::LoadPNG(): image data read" << std::endl)
	}
}

namespace {
const size_t jpeg_input_buffer_size = 4096;

struct data_manager_jpeg_source {
	jpeg_source_mgr pub;
	papki::file* fi;
	JOCTET* buffer;
	bool sof; // true if the file was just opened
};

void jpeg_callback_init_source(j_decompress_ptr cinfo)
{
	ASSERT(cinfo)
	reinterpret_cast<data_manager_jpeg_source*>(cinfo->src)->sof = true;
}

// This function is calld when variable "bytes_in_buffer" reaches 0 and
// the necessarity in new portion of information appears.
// RETURNS: TRUE if the buffer is successfuly filled.
//          FALSE if i/o error occured
boolean jpeg_callback_fill_input_buffer(j_decompress_ptr cinfo)
{
	auto src = reinterpret_cast<data_manager_jpeg_source*>(cinfo->src);
	ASSERT(src)

	// read in JPEGINPUTBUFFERSIZE JOCTET's
	size_t nbytes;

	try {
		auto buf_wrapper = utki::make_span(src->buffer, sizeof(JOCTET) * jpeg_input_buffer_size);
		ASSERT(src->fi)
		nbytes = src->fi->read(buf_wrapper);
	} catch (std::runtime_error&) {
		if (src->sof) {
			return FALSE; // the specified file is empty
		}
		// we read the data before. Insert End Of File info into the buffer
		src->buffer[0] = (JOCTET)(0xFF);
		src->buffer[1] = (JOCTET)(JPEG_EOI);
		nbytes = 2;
	} catch (...) {
		return FALSE; // error
	}

	// Set next input byte for JPEG and number of bytes read
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->sof = false; // the file is not empty since we read some data
	return TRUE; // operation successful
}

// skip num_bytes (seek forward)
void jpeg_callback_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	ASSERT(cinfo)
	auto src = reinterpret_cast<data_manager_jpeg_source*>(cinfo->src);
	ASSERT(src)
	if (num_bytes <= 0) {
		// nothing to skip
		return;
	}

	// read "num_bytes" bytes and waste them away
	while (num_bytes > long(src->pub.bytes_in_buffer)) {
		num_bytes -= long(src->pub.bytes_in_buffer);
		jpeg_callback_fill_input_buffer(cinfo);
	}

	// update current JPEG read position
	src->pub.next_input_byte += size_t(num_bytes);
	src->pub.bytes_in_buffer -= size_t(num_bytes);
}

// terminate source when decompress is finished
// (nothing to do in this function in our case)
void jpeg_callback_term_source(j_decompress_ptr cinfo) {}

} // namespace

// read JPEG function
void raster_image::load_jpg(const papki::file& fi)
{
	ASSERT(!fi.is_open())

	//	TRACE(<< "Image::LoadJPG(): enter" << std::endl)
	if (this->buffer.size()) {
		this->reset();
	}

	papki::file::guard file_guard(fi);
	//	TRACE(<< "Image::LoadJPG(): file opened" << std::endl)

	jpeg_decompress_struct cinfo; // decompression object
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo); // creat decompress object

	data_manager_jpeg_source* src = nullptr;

	// check if memory for JPEG-decompressor manager is allocated,
	// it is possible that several libraries accessing the source
	if (cinfo.src == nullptr) {
		// Allocate memory for our manager and set a pointer of global library
		// structure to it. We use JPEG library memory manager, this means that
		// the library will take care of memory freeing for us.
		// JPOOL_PERMANENT means that the memory is allocated for a whole
		// time  of working with the library.
		cinfo.src = reinterpret_cast<jpeg_source_mgr*>(
			(cinfo.mem->alloc_small)(j_common_ptr(&cinfo), JPOOL_PERMANENT, sizeof(data_manager_jpeg_source))
		);
		src = reinterpret_cast<data_manager_jpeg_source*>(cinfo.src);
		if (!src) {
			throw std::bad_alloc();
		}
		// allocate memory for read data
		src->buffer = reinterpret_cast<JOCTET*>(
			(cinfo.mem->alloc_small)(j_common_ptr(&cinfo), JPOOL_PERMANENT, jpeg_input_buffer_size * sizeof(JOCTET))
		);

		if (!src->buffer) {
			throw std::bad_alloc();
		}

		memset(src->buffer, 0, jpeg_input_buffer_size * sizeof(JOCTET));
	} else {
		src = reinterpret_cast<data_manager_jpeg_source*>(cinfo.src);
	}

	// set handler functions
	src->pub.init_source = &jpeg_callback_init_source;
	src->pub.fill_input_buffer = &jpeg_callback_fill_input_buffer;
	src->pub.skip_input_data = &jpeg_callback_skip_input_data;
	src->pub.resync_to_restart = &jpeg_resync_to_restart; // use default func
	src->pub.term_source = &jpeg_callback_term_source;
	// set the fields of our structure
	src->fi = const_cast<papki::file*>(&fi);
	// set pointers to the buffers
	src->pub.bytes_in_buffer = 0; // forces fill_input_buffer on first read
	src->pub.next_input_byte = nullptr; // until buffer loaded

	jpeg_read_header(&cinfo, TRUE); // read parametrs of a JPEG file

	jpeg_start_decompress(&cinfo); // start decompression

	raster_image::color_depth image_color_depth;
	switch (cinfo.output_components) {
		case 1:
			image_color_depth = raster_image::color_depth::grey;
			break;
		case 2:
			image_color_depth = raster_image::color_depth::grey_alpha;
			break;
		case 3:
			image_color_depth = raster_image::color_depth::rgb;
			break;
		case 4:
			image_color_depth = raster_image::color_depth::rgba;
			break;
		default:
			ASSERT(false, [&](auto& o) {
				o << "Image::LoadJPG(): unknown number of components";
			})
			return;
	}

	// set buffer size (allocate memory for image)
	this->init(r4::vector2<unsigned>(cinfo.output_width, cinfo.output_height), image_color_depth);

	// calculate the size of a row in bytes
	auto num_bytes_in_row = int(this->dims().x() * this->num_channels());

	// Allocate memory for one row. It is an array of rows which
	// contains only one row. JPOOL_IMAGE means that the memory is allocated
	// only for time of this image reading. So, no need to free the memory explicitly.
	JSAMPARRAY buffer = (cinfo.mem->alloc_sarray)(j_common_ptr(&cinfo), JPOOL_IMAGE, num_bytes_in_row, 1);
	memset(*buffer, 0, sizeof(JSAMPLE) * num_bytes_in_row);

	int y = 0;
	while (cinfo.output_scanline < this->dims().y()) {
		// read the string into buffer
		jpeg_read_scanlines(&cinfo, buffer, 1);
		// copy the data to an image
		memcpy(this->buffer.data() + ptrdiff_t(num_bytes_in_row * y), buffer[0], num_bytes_in_row);
		++y;
	}

	jpeg_finish_decompress(&cinfo); // finish file decompression
	jpeg_destroy_decompress(&cinfo); // clean decompression object
}

void raster_image::load(const papki::file& fi)
{
	std::string ext = fi.suffix();

	if (ext == "png") {
		//		TRACE(<< "Image::Load(): loading PNG image" << std::endl)
		this->load_png(fi);
	} else if (ext == "jpg") {
		//		TRACE(<< "Image::Load(): loading JPG image" << std::endl)
		this->load_jpg(fi);
	} else {
		throw std::invalid_argument("Image::Load(): unknown image format");
	}
}
