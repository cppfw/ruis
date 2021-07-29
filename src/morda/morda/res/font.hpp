/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

#include <string>

#include <treeml/tree.hpp>

#include "../resource_loader.hpp"

#include "../fonts/font.hpp"

#include "../util/util.hpp"

namespace morda{ namespace res{

/**
 * @brief %Font resource.
 * 
 * %resource description:
 * 
 * @param normal - file to load normal font from, the True-Type ttf file.
 * @param bold - file to load bold font from, True-Type ttf file. If omitted, the bold font will be the same as normal.
 * @param italic - file to load italic font from, True-Type ttf file. If omitted, the italic font will be the same as normal.
 * @param bold_italic - file to load bold italic font from, True-Type ttf file. If omitted, the bold italic font will be the same as normal.
 * @param chars - list of all chars for which the glyphs should be created.
 * @param size - size of glyphs, in length units, i.e.: no unit(pixels), dp, mm.
 * 
 * Example:
 * @code
 * fnt_normal{
 *     normal {Vera.ttf}
 *     bold {Vera_bold.ttf}
 *     size {12dp}
 * }
 * @endcode
 */
class font : public morda::resource{
	friend class morda::resource_loader;

public:
	enum class style{
		normal,
		bold,
		italic,
		bold_italic
	};

private:
	std::array<std::unique_ptr<const morda::font>, 4> fonts;

public:
	font(
			std::shared_ptr<morda::context> context,
			const papki::file& file_normal,
			std::unique_ptr<const papki::file> file_bold,
			std::unique_ptr<const papki::file> file_italic,
			std::unique_ptr<const papki::file> file_bold_italic,
			unsigned font_size,
			unsigned max_cached
		);

	~font()noexcept{}

	/**
	 * @brief Get font object held by this resource.
	 * @return Font object.
	 */
	const morda::font& get(style font_style = style::normal)const noexcept{
		ASSERT(this->fonts[unsigned(style::normal)])
		const auto& ret = this->fonts[unsigned(font_style)];
		if(ret){
			return *ret;
		}
		return *this->fonts[unsigned(style::normal)];
	}
	
private:
	static std::shared_ptr<font> load(morda::context& ctx, const ::treeml::forest& desc, const papki::file &fi);
};

}}
