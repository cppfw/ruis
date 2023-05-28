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

#pragma once

#include <papki/file.hpp>
#include <r4/vector.hpp>
#include <utki/span.hpp>

namespace morda {

/**
 * @brief Utility class for loading and manipulating raster images.
 */
class raster_image final
{
public:
	/**
	 * @brief Image color depth.
	 */
	enum class color_depth {
		unknown = 0,
		grey = 1, // 1 channel. Only Grey channel
		grey_alpha = 2, // 2 channels. Grey with Alpha channel
		rgb = 3, // 3 channels. Red Green Blue channels
		rgba = 4 // 4 channels. RGBA format (4 channels)
	};

private:
	color_depth color_depth_{color_depth::unknown};
	r4::vector2<unsigned> dims_ = r4::vector2<unsigned>(0);
	std::vector<uint8_t> buffer; // image pixels data

public:
	/**
	 * @brief Default constructor.
	 * Creates uninitialized Image object.
	 */
	raster_image() = default;

	raster_image(const raster_image&) = default;

	/**
	 * @brief Constructor.
	 * Creates image with given parameters, but uninitialized contents.
	 * @param dimensions - image dimensions.
	 * @param pixel_color_depth - color depth.
	 */
	raster_image(r4::vector2<unsigned> dimensions, color_depth pixel_color_depth)
	{
		this->init(dimensions, pixel_color_depth);
	}

	/**
	 * @brief Constructor.
	 * Creates an image with given parameters and initializes image data from given memory buffer.
	 * @param dimensions - image dimensions.
	 * @param pixel_color_depth - color depth.
	 * @param src_buf - pointer to memory buffer to take image data from.
	 */
	raster_image(r4::vector2<unsigned> dimensions, color_depth pixel_color_depth, const uint8_t* src_buf);

	/**
	 * @brief Constructor.
	 * Creates Image object with given dimensions and copies a region from source image to it.
	 * @param pos - position on the source image to copy from.
	 * @param dimensions - dimensions of the area to copy.
	 * @param src - source image to copy area from.
	 */
	raster_image(r4::vector2<unsigned> pos, r4::vector2<unsigned> dimensions, const raster_image& src);

	/**
	 * @brief Constructor.
	 * Creates an image by loading it from file. Supported file types are PNG and JPG.
	 * @param f - file to load image from.
	 */
	raster_image(const papki::file& f)
	{
		this->load(f);
	}

	/**
	 * @brief Get image dimensions.
	 * @return Image dimensions.
	 */
	const r4::vector2<unsigned>& dims() const noexcept
	{
		return this->dims_;
	}

	/**
	 * @brief Get color depth.
	 * @return Bits per pixel.
	 */
	unsigned bits_per_pixel() const
	{
		return this->num_channels() * 8;
	}

	/**
	 * @brief Get color depth.
	 * @return Number of color channels.
	 */
	unsigned num_channels() const
	{
		return unsigned(this->color_depth_);
	}

	/**
	 * @brief Get color depth.
	 * @return Color depth type.
	 */
	color_depth depth() const
	{
		return this->color_depth_;
	}

	/**
	 * @brief Get pixel data.
	 * @return Pixel data of the image.
	 */
	utki::span<uint8_t> pixels()
	{
		return utki::make_span(this->buffer);
	}

	/**
	 * @brief Get pixel data.
	 * @return Pixel data of the image.
	 */
	utki::span<const uint8_t> pixels() const
	{
		return utki::make_span(this->buffer);
	}

public:
	/**
	 * @brief Initialize this image object with given parameters.
	 * Pixel data remains uninitialized.
	 * @param dimensions - image dimensions.
	 * @param pixel_color_depth - color depth.
	 */
	void init(r4::vector2<unsigned> dimensions, color_depth pixel_color_depth);

	/**
	 * @brief Reset this Image object to uninitialized state.
	 * All resources are freed.
	 */
	void reset();

	/**
	 * @brief Fill each image channel with specified value.
	 * @param val - value to use when filling pixel data.
	 */
	void clear(uint8_t val = 0);

	/**
	 * @brief Fill specified color channel with given value.
	 * @param chan - index of color channel to clear.
	 * @param val - value to use for filling.
	 */
	void clear(unsigned chan, uint8_t val = 0);

	/**
	 * @brief Flip image vertically.
	 */
	void flip_vertical();

	/**
	 * @brief Blit another image to this image.
	 * Copy the whole given image to specified location on this image.
	 * @param pos - destination position.
	 * @param src - image to copy to this image.
	 */
	void blit(r4::vector2<unsigned> pos, const raster_image& src);

	/**
	 * @brief Blit another image to this image for desired color channels only.
	 * Copy specified color channel of the whole given image to specified color
	 * channel and specified location on this image.
	 * @param pos - destination position.
	 * @param src - image to copy to this image.
	 * @param dst_chan - index of destination color channel.
	 * @param src_chan - index of source color channel.
	 */
	void blit(r4::vector2<unsigned> pos, const raster_image& src, unsigned dst_chan, unsigned src_chan);

	/**
	 * @brief Get reference to specific channel for given pixel.
	 * @param x - X pixel location.
	 * @param y - Y pixel location.
	 * @param chan - channel index to get reference to.
	 * @return Reference to uint8_t representing a single color channel of given pixel.
	 */
	const uint8_t& pix_chan(unsigned x, unsigned y, unsigned chan) const
	{
		auto i = (y * this->dims().x() + x) * this->num_channels() + chan;
		ASSERT(i < this->buffer.size())
		return this->buffer[i];
	}

	/**
	 * @brief Get reference to specific channel for given pixel.
	 * @param x - X pixel location.
	 * @param y - Y pixel location.
	 * @param chan - channel number to get reference to.
	 * @return Reference to uint8_t representing a single color channel of given pixel.
	 */
	uint8_t& pix_chan(unsigned x, unsigned y, unsigned chan)
	{
		auto i = (y * this->dims().x() + x) * this->num_channels() + chan;
		ASSERT(i < this->buffer.size())
		return this->buffer[i];
	}

	/**
	 * @brief Load image from PNG file.
	 * @param f - PNG file.
	 */
	void load_png(const papki::file& f);

	/**
	 * @brief Load image from JPG file.
	 * @param f - JPG file.
	 */
	void load_jpg(const papki::file& f);

	/**
	 * @brief Load image from file.
	 * It will try to determine the file type from file name.
	 * @param f - file to load image from.
	 */
	void load(const papki::file& f);
};

} // namespace morda
