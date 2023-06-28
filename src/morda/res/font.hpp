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

#include <string>

#include <treeml/tree.hpp>

#include "../fonts/font.hpp"
#include "../resource_loader.hpp"
#include "../util/util.hpp"

namespace morda::res {

/**
 * @brief %Font resource.
 *
 * %resource description:
 *
 * @li @c normal - file to load normal font from, the True-Type ttf file.
 * @li @c bold - file to load bold font from, True-Type ttf file. If omitted, the bold font will be the same as normal.
 * @li @c italic - file to load italic font from, True-Type ttf file. If omitted, the italic font will be the same as
 * normal.
 * @li @c bold_italic - file to load bold italic font from, True-Type ttf file. If omitted, the bold italic font will be
 * the same as normal.
 * @li @c chars - list of all chars for which the glyphs should be created.
 * @li @c size - size of glyphs, in length units, i.e.: no unit(pixels), pp, fp, mm.
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
class font : public morda::resource
{
	friend class morda::resource_loader;

public:
	enum class style {
		normal,
		bold,
		italic,
		bold_italic,

		enum_size
	};

private:
	std::array<std::unique_ptr<const morda::font>, size_t(style::enum_size)> fonts;

public:
	font(
		const utki::shared_ref<morda::context>& context,
		const papki::file& file_normal,
		std::unique_ptr<const papki::file> file_bold,
		std::unique_ptr<const papki::file> file_italic,
		std::unique_ptr<const papki::file> file_bold_italic,
		unsigned font_size,
		unsigned max_cached
	);

	font(const font&) = delete;
	font& operator=(const font&) = delete;

	font(font&&) = delete;
	font& operator=(font&&) = delete;

	~font() override = default;

	/**
	 * @brief Get font object held by this resource.
	 * @return Font object.
	 */
	const morda::font& get(style font_style = style::normal) const noexcept
	{
		ASSERT(this->fonts[unsigned(style::normal)])
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
		const auto& ret = this->fonts[unsigned(font_style)];
		if (ret) {
			return *ret;
		}
		return *this->fonts[unsigned(style::normal)];
	}

private:
	static utki::shared_ref<font> load(
		const utki::shared_ref<morda::context>& ctx,
		const ::treeml::forest& desc,
		const papki::file& fi
	);
};

} // namespace morda::res
