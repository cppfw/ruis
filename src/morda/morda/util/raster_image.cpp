/*
morda GUI framework
Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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

#include <cstring>
#include <algorithm>

#include <png.h>

// JPEG lib does not have 'extern "C"{}' :-(, so we put it outside of their .h
// or will have linking problems otherwise because
// of "_" symbol in front of C-function names
extern "C"{
#include <jpeglib.h>
}

#include "raster_image.hpp"

using namespace morda;

void raster_image::init(r4::vector2<unsigned> dimensions, color_depth typeOfImage){
	this->reset();
	this->dims_v = dimensions;
	this->colorDepth_v = typeOfImage;
	this->buf_v.resize(this->dims().x() * this->dims().y() * this->num_channels());
}

raster_image::raster_image(r4::vector2<unsigned> dimensions, color_depth typeOfImage, const std::uint8_t* srcBuf){
	ASSERT(srcBuf)
	this->init(dimensions, typeOfImage);
	memcpy(&*this->buf_v.begin(), srcBuf, this->buf_v.size() * sizeof(this->buf_v[0]));
}

raster_image::raster_image(r4::vector2<unsigned> pos, r4::vector2<unsigned> dimensions, const raster_image& src){
	if(src.dims().x() == 0 || src.dims().y() == 0){
		throw std::invalid_argument("Image::Image(): source image has zero dimensions");
	}

	if(src.dims().x() <= pos.x() || src.dims().y() <= pos.y() || src.dims().x() < (pos.x() + dimensions.x()) || src.dims().y() < (pos.y() + dimensions.y())){
		throw std::invalid_argument("Image::Image(): incorrect dimensions of given images");
	}

	this->init(dimensions, src.depth());

	// copy image data
	throw std::runtime_error("Image::Image(unsigned x, unsigned y, unsigned width, unsigned height, const Image& src): is not implemented");
	//for(unsigned j=0; j<this->H(); ++j)
	//    memcpy(data+j*w*channels,src->data+((j+y)*src->w+x)*channels,w*channels);
}

// fills image buffer with zeroes
void raster_image::clear(std::uint8_t val){
	if (this->buf_v.size() == 0) {
		return;
	}
	memset(&*this->buf_v.begin(), val, this->buf_v.size() * sizeof(this->buf_v[0]));
}

void raster_image::clear(unsigned chan, std::uint8_t val){
	for(unsigned i = 0; i < this->dims().x() * this->dims().y(); ++i){
		this->buf_v[i * this->num_channels() + chan] = val;
	}
}

void raster_image::reset(){
	this->dims_v.set(0);
	this->colorDepth_v = color_depth::unknown;
	this->buf_v.clear();
}

void raster_image::flip_vertical(){
	if(!this->buf_v.size()){
		// nothing to flip
		return;
	}

	unsigned stride = this->num_channels() * this->dims().x(); // stride
	std::vector<std::uint8_t> line(stride);

	//TODO: use iterators
	for(unsigned i = 0; i < this->dims().y() / 2; ++i){
		memcpy(&*line.begin(), &*this->buf_v.begin() + stride * i, stride); // move line to temp
		memcpy(&*this->buf_v.begin() + stride * i, &*this->buf_v.begin() + stride * (this->dims().y() - i - 1), stride); // move bottom line to top
		memcpy(&*this->buf_v.begin() + stride * (this->dims().y() - i - 1), &*line.begin(), stride);
	}
}

void raster_image::blit(r4::vector2<unsigned> pos, const raster_image& src){
	ASSERT(this->buf_v.size() != 0)
	if(this->depth() != src.depth()){
		throw std::invalid_argument("Image::Blit(): bits per pixel values do not match");
	}

	using std::min;

	auto blit_area = min(src.dims(), this->dims() - pos);

	//TODO: implement blitting for all image types
	switch(this->depth()){
		case color_depth::grey:
			for(unsigned j = 0; j < blit_area.y(); ++j){
				for(unsigned i = 0; i < blit_area.x(); ++i){
					this->pix_chan(i + pos.x(), j + pos.y(), 0) = src.pix_chan(i, j, 0);
				}
			}
			break;
		case color_depth::grey_alpha:
			for(unsigned j = 0; j < blit_area.y(); ++j){
				for(unsigned i = 0; i < blit_area.x(); ++i){
					this->pix_chan(i + pos.x(), j + pos.y(), 0) = src.pix_chan(i, j, 0);
					this->pix_chan(i + pos.x(), j + pos.y(), 1) = src.pix_chan(i, j, 1);
				}
			}
			break;
		default:
			ASSERT_INFO(false, "Image::Blit(): unknown image type")
			break;
	}
}

void raster_image::blit(r4::vector2<unsigned> pos, const raster_image& src, unsigned dstChan, unsigned srcChan){
	ASSERT(this->buf_v.size())
	if(dstChan >= this->num_channels()){
		throw std::invalid_argument("Image::Blit(): destination channel index is greater than number of channels in the image");
	}

	if(srcChan >= src.num_channels()){
		throw std::invalid_argument("Image::Blit(): source channel index is greater than number of channels in the image");
	}

	using std::min;

	auto blit_area = min(src.dims(), this->dims() - pos);

	for(unsigned j = 0; j < blit_area.y(); ++j){
		for(unsigned i = 0; i < blit_area.x(); ++i){
			this->pix_chan(i + pos.x(), j + pos.y(), dstChan) = src.pix_chan(i, j, srcChan);
		}
	}
}

// custom file read function for PNG
namespace{
void PNG_CustomReadFunction(png_structp pngPtr, png_bytep data, png_size_t length){
	papki::file* fi = reinterpret_cast<papki::file*>(png_get_io_ptr(pngPtr));
	ASSERT(fi)
//	TRACE(<< "PNG_CustomReadFunction: fi = " << fi << " pngPtr = " << pngPtr << " data = " << std::hex << data << " length = " << length << std::endl)
	try{
		auto bufWrapper = utki::make_span(data, size_t(length));
		fi->read(bufWrapper);
//		TRACE(<< "PNG_CustomReadFunction: fi->Read() finished" << std::endl)
	}catch(...){
		// do not let any exception get out of this function
//		TRACE(<< "PNG_CustomReadFunction: fi->Read() failed" << std::endl)
	}
}
}

// read PNG file
void raster_image::load_png(const papki::file& fi){
	ASSERT(!fi.is_open())

	if(this->buf_v.size() > 0){
		this->reset();
	}

	papki::file::guard file_guard(fi);
//	TRACE(<< "Image::LoadPNG(): file opened" << std::endl)
	
	const unsigned png_sig_size = 8; // the size of PNG signature (max 8 bytes)

	{	
		std::array<png_byte, png_sig_size> sig;
		auto span = utki::make_span(sig);

		memset(span.data(), 0, span.size_bytes());
		
		auto num_bytes_read = fi.read(span);
		if(num_bytes_read != span.size_bytes()){
			throw std::invalid_argument("raster_image::load_png(): could not read file signature");
		}

		// check that it is a PNG file
		if(png_sig_cmp(span.data(), 0, span.size_bytes()) != 0){
			throw std::invalid_argument("raster_image::load_png(): not a PNG file");
		}
	}

//	TRACE(<< "Image::LoadPNG(): file is a PNG" << std::endl)

	// create internal PNG-structure to work with PNG file
	// (no warning and error callbacks)
	png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	utki::scope_exit png_scope_exit([&pngPtr](){
		png_destroy_read_struct(&pngPtr, 0, 0); // free libpng memory
	});

	// NOTE: the memory freed by png_destroy_read_struct()
	png_infop infoPtr = png_create_info_struct(pngPtr);
	
	png_set_sig_bytes(pngPtr, png_sig_size); // we've already read png_sig_size bytes

	// set custom "ReadFromFile" function
	png_set_read_fn(pngPtr, const_cast<papki::file*>(&fi), PNG_CustomReadFunction);

	png_read_info(pngPtr, infoPtr); // read in all information about file

	// get information from infoPtr
	png_uint_32 width = 0;
	png_uint_32 height = 0;
	int bitDepth = 0;
	int colorType = 0;
	png_get_IHDR(pngPtr, infoPtr, &width, &height, &bitDepth, &colorType, 0, 0, 0);

	// strip 16-bit png to 8-bit
	if(bitDepth == 16){
		png_set_strip_16(pngPtr);
	}
	// convert paletted PNG to rgb image
	if(colorType == PNG_COLOR_TYPE_PALETTE){
		png_set_palette_to_rgb(pngPtr);
	}
	// convert grayscale PNG to 8bit greyscale PNG
	if(colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8){
		png_set_expand_gray_1_2_4_to_8(pngPtr);
	}

	//TODO: is this needed? remove?
	//if(png_get_valid(pngPtr, infoPtr,PNG_INFO_tRNS)) png_set_tRNS_to_alpha(pngPtr);

	// set gamma information
	double gamma = 0.0f;

	// if there's gamma info in the file, set it to 2.2
	if(png_get_gAMA(pngPtr, infoPtr, &gamma)){
		png_set_gamma(pngPtr, 2.2, gamma);
	}else{
		png_set_gamma(pngPtr, 2.2, 0.45455); // set to 0.45455 otherwise (good guess for GIF images on PCs)
	}

	// update info after all transformations
	png_read_update_info(pngPtr, infoPtr);
	// get all dimensions and color info again
	png_get_IHDR(pngPtr, infoPtr, &width, &height, &bitDepth, &colorType, 0, 0, 0);
	ASSERT(bitDepth == 8)

	// set image type
	raster_image::color_depth imageType;
	switch(colorType){
		case PNG_COLOR_TYPE_GRAY:
			imageType = raster_image::color_depth::grey;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			imageType = raster_image::color_depth::grey_alpha;
			break;
		case PNG_COLOR_TYPE_RGB:
			imageType = raster_image::color_depth::rgb;
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			imageType = raster_image::color_depth::rgba;
			break;
		default:
			throw std::runtime_error("Image::LoadPNG(): unknown colorType");
	}

	// set image dimensions and set buffer size
	this->init(r4::vector2<unsigned>(width, height), imageType); // set buf array size (allocate memory)

//	TRACE(<< "Image::LoadPNG(): memory for image allocated" << std::endl)

	// read image data
	png_size_t bytesPerRow = png_get_rowbytes(pngPtr, infoPtr); // get bytes per row

	// check that our expectations are correct
	if(bytesPerRow != this->dims().x() * this->num_channels()){
		throw std::runtime_error("Image::LoadPNG(): number of bytes per row does not match expected value");
	}

	ASSERT((bytesPerRow * height) == this->buf_v.size())

//	TRACE(<< "Image::LoadPNG(): going to read in the data" << std::endl)
	{
		ASSERT(this->dims().y() && this->buf_v.size())
		std::vector<png_bytep> rows(this->dims().y());
		// initialize row pointers
//		M_IMAGE_PRINT(<< "Image::LoadPNG(): this->buf.Buf() = " << std::hex << this->buf.Buf() << std::endl)
		for(unsigned i = 0; i < this->dims().y(); ++i){
			rows[i] = &*this->buf_v.begin() + i * bytesPerRow;
//			M_IMAGE_PRINT(<< "Image::LoadPNG(): rows[i] = " << std::hex << rows[i] << std::endl)
		}
//		TRACE(<< "Image::LoadPNG(): row pointers are set" << std::endl)
		// read in image data
		png_read_image(pngPtr, &*rows.begin());
//		TRACE(<< "Image::LoadPNG(): image data read" << std::endl)
	}
}

namespace{
const size_t DJpegInputBufferSize = 4096;

struct DataManagerJPEGSource{
	jpeg_source_mgr pub;
	papki::file *fi;
	JOCTET *buffer;
	bool sof; // true if the file was just opened
};

void JPEG_InitSource(j_decompress_ptr cinfo){
	ASSERT(cinfo)
	reinterpret_cast<DataManagerJPEGSource*>(cinfo->src)->sof = true;
}

// This function is calld when variable "bytes_in_buffer" reaches 0 and
// the necessarity in new portion of information appears.
// RETURNS: TRUE if the buffer is successfuly filled.
//          FALSE if i/o error occured
boolean JPEG_FillInputBuffer(j_decompress_ptr cinfo){
	DataManagerJPEGSource* src = reinterpret_cast<DataManagerJPEGSource*>(cinfo->src);
	ASSERT(src)

	// read in JPEGINPUTBUFFERSIZE JOCTET's
	size_t nbytes;

	try{
		auto bufWrapper = utki::make_span(src->buffer, sizeof(JOCTET) * DJpegInputBufferSize);
		ASSERT(src->fi)
		nbytes = src->fi->read(bufWrapper);
	}catch(std::runtime_error&){
		if(src->sof){
			return FALSE; // the specified file is empty
		}
		// we read the data before. Insert End Of File info into the buffer
		src->buffer[0] = (JOCTET)(0xFF);
		src->buffer[1] = (JOCTET)(JPEG_EOI);
		nbytes = 2;
	}catch(...){
		return FALSE; // error
	}

	// Set next input byte for JPEG and number of bytes read
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->sof = false; // the file is not empty since we read some data
	return TRUE; // operation successful
}

// skip num_bytes (seek forward)
void JPEG_SkipInputData(j_decompress_ptr cinfo, long numBytes){
	ASSERT(cinfo)
	DataManagerJPEGSource* src = reinterpret_cast<DataManagerJPEGSource*>(cinfo->src);
	ASSERT(src)
	if(numBytes <= 0){
		// nothing to skip
		return;
	}

	// read "numBytes" bytes and waste them away
	while(numBytes > long(src->pub.bytes_in_buffer)){
		numBytes -= long(src->pub.bytes_in_buffer);
		JPEG_FillInputBuffer(cinfo);
	}

	// update current JPEG read position
	src->pub.next_input_byte += size_t(numBytes);
	src->pub.bytes_in_buffer -= size_t(numBytes);
}

// terminate source when decompress is finished
// (nothing to do in this function in our case)
void JPEG_TermSource(j_decompress_ptr cinfo){}

}

// read JPEG function
void raster_image::load_jpg(const papki::file& fi){
	ASSERT(!fi.is_open())

//	TRACE(<< "Image::LoadJPG(): enter" << std::endl)
	if(this->buf_v.size()){
		this->reset();
	}
	
	papki::file::guard file_guard(fi);
//	TRACE(<< "Image::LoadJPG(): file opened" << std::endl)

	jpeg_decompress_struct cinfo; // decompression object
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo); // creat decompress object

	DataManagerJPEGSource* src = 0;

	// check if memory for JPEG-decompressor manager is allocated,
	// it is possible that several libraries accessing the source
	if(cinfo.src == 0){
		// Allocate memory for our manager and set a pointer of global library
		// structure to it. We use JPEG library memory manager, this means that
		// the library will take care of memory freeing for us.
		// JPOOL_PERMANENT means that the memory is allocated for a whole
		// time  of working with the library.
		cinfo.src = reinterpret_cast<jpeg_source_mgr*>(
				(cinfo.mem->alloc_small)(
						j_common_ptr(&cinfo),
						JPOOL_PERMANENT,
						sizeof(DataManagerJPEGSource)
					)
			);
		src = reinterpret_cast<DataManagerJPEGSource*>(cinfo.src);
		if(!src){
			throw std::bad_alloc();
		}
		// allocate memory for read data
		src->buffer = reinterpret_cast<JOCTET*>(
				(cinfo.mem->alloc_small)(
						j_common_ptr(&cinfo),
						JPOOL_PERMANENT,
						DJpegInputBufferSize * sizeof(JOCTET)
					)
			);

		if(!src->buffer){
			throw std::bad_alloc();
		}

		memset(src->buffer, 0, DJpegInputBufferSize * sizeof(JOCTET));
	}else{
		src = reinterpret_cast<DataManagerJPEGSource*>(cinfo.src);
	}

	// set handler functions
	src->pub.init_source = &JPEG_InitSource;
	src->pub.fill_input_buffer = &JPEG_FillInputBuffer;
	src->pub.skip_input_data = &JPEG_SkipInputData;
	src->pub.resync_to_restart = &jpeg_resync_to_restart; // use default func
	src->pub.term_source = &JPEG_TermSource;
	// set the fields of our structure
	src->fi = const_cast<papki::file*>(&fi);
	// set pointers to the buffers
	src->pub.bytes_in_buffer = 0; // forces fill_input_buffer on first read
	src->pub.next_input_byte = 0; // until buffer loaded

	jpeg_read_header(&cinfo, TRUE); // read parametrs of a JPEG file

	jpeg_start_decompress(&cinfo); // start decompression

	raster_image::color_depth imageType;
	switch(cinfo.output_components){
		case 1:
			imageType = raster_image::color_depth::grey;
			break;
		case 2:
			imageType = raster_image::color_depth::grey_alpha;
			break;
		case 3:
			imageType = raster_image::color_depth::rgb;
			break;
		case 4:
			imageType = raster_image::color_depth::rgba;
			break;
		default:
			ASSERT_INFO(false, "Image::LoadJPG(): unknown number of components")
			return;
	}
	
	// set buffer size (allocate memory for image)
	this->init(r4::vector2<unsigned>(cinfo.output_width, cinfo.output_height), imageType);

	// calculate the size of a row in bytes
	int bytesRow = this->dims().x() * this->num_channels();

	// Allocate memory for one row. It is an array of rows which
	// contains only one row. JPOOL_IMAGE means that the memory is allocated
	// only for time of this image reading. So, no need to free the memory explicitly.
	JSAMPARRAY buffer = (cinfo.mem->alloc_sarray)(
			j_common_ptr(&cinfo),
			JPOOL_IMAGE,
			bytesRow,
			1
		);
	memset(*buffer, 0, sizeof(JSAMPLE) * bytesRow);

	int y = 0;
	while(cinfo.output_scanline < this->dims().y()){
		// read the string into buffer
		jpeg_read_scanlines(&cinfo, buffer, 1);
		// copy the data to an image
		memcpy(&*this->buf_v.begin() + bytesRow * y, buffer[0], bytesRow);
		++y;
	}

	jpeg_finish_decompress(&cinfo); // finish file decompression
	jpeg_destroy_decompress(&cinfo); // clean decompression object
}

void raster_image::load(const papki::file& fi){
	std::string ext = fi.suffix();

	if(ext == "png"){
//		TRACE(<< "Image::Load(): loading PNG image" << std::endl)
		this->load_png(fi);
	}else if(ext == "jpg"){
//		TRACE(<< "Image::Load(): loading JPG image" << std::endl)
		this->load_jpg(fi);
	}else{
		throw std::invalid_argument("Image::Load(): unknown image format");
	}
}
