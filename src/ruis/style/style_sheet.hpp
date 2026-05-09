/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include <tml/tree.hpp>
#include <utki/enum_array.hpp>

namespace ruis {

/**
 * @brief Style value id.
 * Ruis standard style values.
 */
enum class style {
	color_background,
	color_middleground,
	color_foreground,

	color_text_normal,
	color_text_selection_bg,

	color_highlight,

	color_cursor,

	dim_tree_view_item_indent,

	font_size_normal,
	font_face_normal,

	enum_size
};

/**
 * @brief Style sheet.
 * TODO: write more description
 * File format:
 * @verbatim
 * version{<version-number>}
 * ruis{
 *     // ruis standard style values
 *     <style-name>{<style-value>}
 *     ...
 * }
 * user{
 *     // user defined style values
 *     <style-name>{<style-value>}
 *     ...
 * }
 * @endverbatim
 */
class style_sheet
{
	utki::enum_array<tml::forest, style> standard_styles;
	std::map<std::string, tml::forest, std::less<>> user_styles;

	void parse(tml::forest desc);

	static style name_to_style(std::string_view name);

public:
	style_sheet() = default;

	style_sheet(tml::forest desc);

	/**
	 * @brief Get style value description.
	 * @param style_id - id of the style value.
	 * @return pointer to the style value description if present in the style sheet.
	 * @return nullptr in case the style id is not present in the style sheet.
	 */
	const tml::forest* get(std::string_view style_id) const noexcept;

	/**
	 * @brief Get standard style value description.
	 * @param style_id - id of the standard style value.
	 * @return reference to the standard style value description.
	 */
	const tml::forest& get(style style_id) const noexcept
	{
		return this->standard_styles[style_id];
	}

	static style_sheet load(const fsif::file& fi);
};

} // namespace ruis