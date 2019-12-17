#pragma once

#include <utki/exception.hpp>

#include <papki/File.hpp>

#include <r4/vector2.hpp>



namespace morda{


/**
 * @brief Utility class for loading and manipulating raster images.
 */
class RasterImage final{
public:
	/**
	 * @brief Image color depth.
	 */
	enum class ColorDepth_e{
		UNKNOWN = 0,
		GREY  = 1, //1 channel. Only Grey channel
		GREYA = 2, //2 channels. Grey with Alpha channel
		RGB   = 3, //3 channels. Red Green Blue channels
		RGBA  = 4  //4 channels. RGBA format (4 channels)
	};

	/**
	 * @brief Basic image exception.
	 */
	class exception : public utki::exception{
	public:
		exception(const std::string& message) :
				utki::exception(message)
		{}
	};

private:
	ColorDepth_e colorDepth_v;
	r4::vec2ui dim_v = r4::vec2ui(0);
	std::vector<std::uint8_t> buf_v;//image pixels data

public:
	/**
	 * @brief Default constructor.
	 * Creates uninitialized Image object.
	 */
	RasterImage() :
			colorDepth_v(ColorDepth_e::UNKNOWN)
	{}

	RasterImage(const RasterImage& im) = default;

	/**
	 * @brief Constructor.
	 * Creates image with given parameters, but uninitialized contents.
	 * @param dimensions - image dimensions.
	 * @param colorDepth - color depth.
	 */
	RasterImage(r4::vec2ui dimensions, ColorDepth_e colorDepth){
		this->init(dimensions, colorDepth);
	}

	/**
	 * @brief Constructor.
	 * Creates an image with given parameters and initializes image data from given memory buffer.
	 * @param dimensions - image dimensions.
	 * @param colorDepth - color depth.
	 * @param srcBuf - pointer to memory buffer to take image data from.
	 */
	RasterImage(r4::vec2ui dimensions, ColorDepth_e colorDepth, const std::uint8_t* srcBuf);

	/**
	 * @brief Constructor.
	 * Creates Image object with given dimensions and copies a region from source image to it.
	 * @param pos - position on the source image to copy from.
	 * @param dimensions - dimensions of the area to copy.
	 * @param src - source image to copy area from.
	 */
	RasterImage(r4::vec2ui pos, r4::vec2ui dimensions, const RasterImage& src);

	/**
	 * @brief Constructor.
	 * Creates an image by loading it from file. Supported file types are PNG and JPG.
	 * @param f - file to load image from.
	 */
	RasterImage(const papki::File& f){
		this->load(f);
	}

	/**
	 * @brief Get image dimensions.
	 * @return Image dimensions.
	 */
	const r4::vec2ui& dim()const noexcept{
		return this->dim_v;
	}

	/**
	 * @brief Get color depth.
	 * @return Bits per pixel.
	 */
	unsigned bitsPerPixel()const{
		return this->numChannels() * 8;
	}

	/**
	 * @brief Get color depth.
	 * @return Number of color channels.
	 */
	unsigned numChannels()const{
		return unsigned(this->colorDepth_v);
	}

	/**
	 * @brief Get color depth.
	 * @return Color depth type.
	 */
	ColorDepth_e colorDepth()const{
		return this->colorDepth_v;
	}

	/**
	 * @brief Get pixel data.
	 * @return Pixel data of the image.
	 */
	utki::Buf<std::uint8_t> buf(){
		return utki::wrapBuf(this->buf_v);
	}

	/**
	 * @brief Get pixel data.
	 * @return Pixel data of the image.
	 */
	const utki::Buf<std::uint8_t> buf()const{
		return utki::wrapBuf(this->buf_v);
	}

public:
	/**
	 * @brief Initialize this image object with given parameters.
	 * Pixel data remains uninitialized.
	 * @param dimensions - image dimensions.
	 * @param colorDepth - color depth.
	 */
	void init(r4::vec2ui dimensions, ColorDepth_e colorDepth);


	/**
	 * @brief Reset this Image object to uninitialized state.
	 * All resources are freed.
	 */
	void reset();

	/**
	 * @brief Fill each image channel with specified value.
	 * @param val - value to use when filling pixel data.
	 */
	void clear(std::uint8_t  val = 0);

	/**
	 * @brief Fill specified color channel with given value.
	 * @param chan - index of color channel to clear.
	 * @param val - value to use for filling.
	 */
	void clear(unsigned chan, std::uint8_t val = 0);

	/**
	 * @brief Flip image vertically.
	 */
	void flipVertical();

	/**
	 * @brief Blit another image to this image.
	 * Copy the whole given image to specified location on this image.
	 * @param x - destination X location.
	 * @param y - destination Y location.
	 * @param src - image to copy to this image.
	 */
	void blit(unsigned x, unsigned y, const RasterImage& src);


	/**
	 * @brief Blit another image to this image for desired color channels only.
	 * Copy specified color channel of the whole given image to specified color
	 * channel and specified location on this image.
	 * @param x - destination X location.
	 * @param y - destination Y location.
	 * @param src - image to copy to this image.
	 * @param dstChan - index of destination color channel.
	 * @param srcChan - index of source color channel.
	 */
	void blit(unsigned x, unsigned y, const RasterImage& src, unsigned dstChan, unsigned srcChan);

	/**
	 * @brief Get reference to specific channel for given pixel.
	 * @param x - X pixel location.
	 * @param y - Y pixel location.
	 * @param chan - channel index to get reference to.
	 * @return Reference to uint8_t representing a single color channel of given pixel.
	 */
	const std::uint8_t& pixChan(unsigned x, unsigned y, unsigned chan)const{
		auto i = (y * this->dim().x + x) * this->numChannels() + chan;
		ASSERT(i < this->buf_v.size())
		return this->buf_v[i];
	}

	/**
	 * @brief Get reference to specific channel for given pixel.
	 * @param x - X pixel location.
	 * @param y - Y pixel location.
	 * @param chan - channel number to get reference to.
	 * @return Reference to uint8_t representing a single color channel of given pixel.
	 */
	std::uint8_t& pixChan(unsigned x, unsigned y, unsigned chan){
		auto i = (y * this->dim().x + x) * this->numChannels() + chan;
		ASSERT(i < this->buf_v.size())
		return this->buf_v[i];
	}

	/**
	 * @brief Load image from PNG file.
	 * @param f - PNG file.
	 */
	void loadPNG(const papki::File& f);//Load image from PNG-file

	/**
	 * @brief Load image from JPG file.
	 * @param f - JPG file.
	 */
	void loadJPG(const papki::File& f);//Load image from JPG-file

//	void loadTGA(papki::File& f);//Load image from TGA-file

	/**
	 * @brief Load image from file.
	 * It will try to determine the file type from file name.
	 * @param f - file to load image from.
	 */
	void load(const papki::File& f);
};



}
