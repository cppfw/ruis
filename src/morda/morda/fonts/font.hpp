#pragma once

#include <string>

#include <utki/unicode.hpp>

#include <r4/matrix.hpp>
#include <r4/rectangle.hpp>

#include "../config.hpp"

#include "../context.hpp"

namespace morda{

/**
 * @brief Basic class representing a font.
 */
class font{
public:
	const std::shared_ptr<morda::context> context;
protected:
	/**
	 * @brief Distance between lines of text.
	 */
	real height;
	
	real descender;
	
	real ascender;
	
	font(std::shared_ptr<morda::context> context) :
			context(std::move(context))
	{}
	
	font(const font&) = delete;
	font& operator=(const font&) = delete;
	
public:

	/**
	 * @brief String rendering result.
	 */
	struct render_result{
		/**
		 * @brief String advance.
		 */
		real advance;

		/**
		 * @brief String length in characters.
		 * This only makes sence for monospaced fonts.
		 * This takes tabulations into account.
		 */
		size_t offset;
	};

protected:

	/**
	 * @brief Render string of text.
	 * @param matrix - transformation matrix to use when rendering the text.
	 * @param color - text color.
	 * @param str - string of text to render.
	 * @param tab_size - tab size in characters. For non-monospace fonts this is the number of space-character advances.
	 * @param offset - in case the string rendered is a sub-string of a bigger string, this is a sub-string offset from the string start.
	 *                 This is only for monospaced fonts.
	 *                 This is used to calculated proper tab size as it depends on at which place of the string it appears.
	 * @return An advance to the end of the rendered text string. It can be used to position the next text string when rendering.
	 */
	virtual render_result render_internal(
			const morda::matrix4& matrix,
			r4::vector4<float> color,
			const std::u32string_view str,
			size_t tab_size,
			size_t offset
		)const = 0;
	
	/**
	 * @brief Get string advance.
	 * @param str - string of text to get advance for.
	 * @return Advance of the text string.
	 */
	virtual real get_advance_internal(const std::u32string& str)const = 0;
	
	/**
	 * @brief Get bounding box of the string.
	 * @param str - string of text to get the bounding box for.
	 * @return Bounding box of the text string.
	 */
	virtual morda::rectangle get_bounding_box_internal(const std::u32string& str)const = 0;
public:
	virtual ~font()noexcept{}
	
	/**
	 * @brief Render string of text.
	 * @param matrix - transformation matrix to use when rendering.
	 * @param color - text color.
	 * @param str - string of text to render.
	 * @param tab_size - tab size in characters. For non-monospace fonts this is the number of space-character advances.
	 * @param offset - in case the string rendered is a sub-string of a bigger string, this is a sub-string offset from the string start.
	 *                 This is only for monospaced fonts.
	 *                 This is used to calculated proper tab size as it depends on at which place of the string it appears.
	 * @return Render result.
	 */
	render_result render(
			const morda::matrix4& matrix,
			r4::vector4<float> color,
			const std::u32string_view str,
			size_t tab_size = 4,
			size_t offset = 0
		)const
	{
		return this->render_internal(matrix, color, str, tab_size, offset);
	}

	/**
	 * @brief Render string of text.
	 * @param matrix - transformation matrix to use when rendering.
	 * @param color - text color.
	 * @param str - string of text to render.
	 * @param tab_size - tab size in characters. For non-monospace fonts this is the number of space-character advances.
	 * @param offset - in case the string rendered is a sub-string of a bigger string, this is a sub-string offset from the string start.
	 *                 This is only for monospaced fonts.
	 *                 This is used to calculated proper tab size as it depends on at which place of the string it appears.
	 * @return Render result.
	 */
	render_result render(
			const morda::matrix4& matrix,
			r4::vector4<float> color,
			utki::utf8_iterator str,
			size_t tab_size = 4,
			size_t offset = 0
		)const
	{
		return this->render(matrix, color, utki::to_utf32(str), tab_size, offset);
	}
	
	/**
	 * @brief Render string of text.
	 * @param matrix - transformation matrix to use when rendering.
	 * @param color - text color.
	 * @param str - string of text to render.
	 * @param tab_size - tab size in characters. For non-monospace fonts this is the number of space-character advances.
	 * @param offset - in case the string rendered is a sub-string of a bigger string, this is a sub-string offset from the string start.
	 *                 This is only for monospaced fonts.
	 *                 This is used to calculated proper tab size as it depends on at which place of the string it appears.
	 * @return Render result.
	 */
	render_result render(
			const morda::matrix4& matrix,
			r4::vector4<float> color,
			const char* str,
			size_t tab_size = 4,
			size_t offset = 0
		)const
	{
		return this->render(matrix, color, utki::utf8_iterator(str));
	}
	
	/**
	 * @brief Render string of text.
	 * @param matrix - transformation matrix to use when rendering.
	 * @param color - text color.
	 * @param str - string of text to render.
	 * @param tab_size - tab size in characters. For non-monospace fonts this is the number of space-character advances.
	 * @param offset - in case the string rendered is a sub-string of a bigger string, this is a sub-string offset from the string start.
	 *                 This is only for monospaced fonts.
	 *                 This is used to calculated proper tab size as it depends on at which place of the string it appears.
	 * @return Redner result.
	 */
	render_result render(
			const morda::matrix4& matrix,
			r4::vector4<float> color,
			const std::string& str,
			size_t tab_size = 4,
			size_t offset = 0
		)const
	{
		return this->render(matrix, color, str.c_str(), tab_size, offset);
	}

	/**
	 * @brief Get string advance.
	 * @param str - string to get advance for.
	 * @return Advance of the string of text.
	 */
	real get_advance(utki::utf8_iterator str)const{
		return this->get_advance_internal(utki::to_utf32(str));
	}
	
	/**
	 * @brief Get string advance.
	 * @param str - string to get advance for.
	 * @return Advance of the string of text.
	 */
	real get_advance(const std::u32string& str)const{
		return this->get_advance_internal(str);
	}
	
	/**
	 * @brief Get string advance.
	 * @param str - string to get advance for.
	 * @return Advance of the string of text.
	 */
	real get_advance(const char* str)const{
		return this->get_advance(utki::utf8_iterator(str));
	}
	
	/**
	 * @brief Get string advance.
	 * @param str - string to get advance for.
	 * @return Advance of the string of text.
	 */
	real get_advance(const std::string& str)const{
		return this->get_advance(str.c_str());
	}

	/**
	 * @brief Get advance of the character.
	 * @param c - character to get advance for.
	 * @return Advance of the character.
	 */
	virtual real get_advance(char32_t c)const = 0;

	/**
	 * @brief Get bounding box of the string.
	 * @param str - string of text to get the bounding box for.
	 * @return Bounding box of the text string.
	 */
	morda::rectangle get_bounding_box(utki::utf8_iterator str)const{
		return this->get_bounding_box_internal(utki::to_utf32(str));
	}
	
	/**
	 * @brief Get bounding box of the string.
	 * @param str - string of text to get the bounding box for.
	 * @return Bounding box of the text string.
	 */
	morda::rectangle get_bounding_box(const std::u32string& str)const{
		return this->get_bounding_box_internal(str);
	}
	
	/**
	 * @brief Get bounding box of the string.
	 * @param str - string of text to get the bounding box for.
	 * @return Bounding box of the text string.
	 */
	morda::rectangle get_bounding_box(const char* str)const{
		return this->get_bounding_box(utki::utf8_iterator(str));
	}
	
	/**
	 * @brief Get bounding box of the string.
	 * @param str - string of text to get the bounding box for.
	 * @return Bounding box of the text string.
	 */
	morda::rectangle get_bounding_box(const std::string& str)const{
		return this->get_bounding_box(str.c_str());
	}
	
	/**
	 * @brief Get height of the font.
	 * Height of the font is the distance normally used between lines of text.
	 * @return Height of the font.
	 */
	real get_height()const noexcept{
		return this->height;
	}
	
	real get_descender()const noexcept{
		return this->descender;
	}
	
	real get_ascender()const noexcept{
		return this->ascender;
	}
};

}
