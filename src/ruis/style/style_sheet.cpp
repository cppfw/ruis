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

#include "style_sheet.hpp"

using namespace std::string_view_literals;
using namespace ruis;

style style_sheet::name_to_style(std::string_view name)
{
	if (name == "color_background"sv) {
		return style::color_background;
	} else if (name == "color_middleground"sv) {
		return style::color_middleground;
	} else if (name == "color_foreground"sv) {
		return style::color_foreground;
	} else if (name == "color_text_normal"sv) {
		return style::color_text_normal;
	} else if (name == "color_text_selection_bg"sv) {
		return style::color_text_selection_bg;
	} else if (name == "color_highlight"sv) {
		return style::color_highlight;
	} else if (name == "color_cursor"sv) {
		return style::color_cursor;
	} else if (name == "dim_tree_view_item_indent"sv) {
		return style::dim_tree_view_item_indent;
	} else if (name == "font_size_normal"sv) {
		return style::font_size_normal;
	} else if (name == "font_face_normal"sv) {
		return style::font_face_normal;
	}

	throw std::invalid_argument(utki::cat("style_sheet::name_to_style(name): unknown style name: ", name));
}

style_sheet::style_sheet(tml::forest desc)
{
	this->parse(std::move(desc));
}

void style_sheet::parse(tml::forest desc)
{
	if (desc.empty()) {
		throw std::invalid_argument("style_sheet::parse(desc): empty style sheet description supplied");
	}

	const auto& version_node = desc.front();
	if (version_node.value.string != "version"sv) {
		throw std::invalid_argument("style_sheet::parse(desc): 'version' expected as a first entry");
	}

	if (version_node.children.empty()) {
		throw std::invalid_argument("style_sheet::parse(desc): 'version' field is empty");
	}

	auto version = version_node.children.front().value.to_uint32();

	if (version != 1) {
		throw std::invalid_argument(utki::cat(
			"style_sheet::parse(desc): unsupported file format version. expected 1, got: ", //
			version
		));
	}

	for (auto& d : utki::skip_front<1>(desc)) {
		if (d.value.string == "ruis"sv) {
			for (auto& s : d.children) {
				this->standard_styles[style_sheet::name_to_style(s.value.string)] = std::move(s.children);
			}
		} else if (d.value.string == "user"sv) {
			for (auto& s : d.children) {
				this->user_styles.insert_or_assign(
					std::move(s.value.string), //
					std::move(s.children)
				);
			}
		}
	}
}

style_sheet style_sheet::load(const fsif::file& fi)
{
	return {tml::read(fi)};
}

const tml::forest* style_sheet::get(std::string_view style_id) const noexcept
{
	auto i = this->user_styles.find(style_id);
	if (i == this->user_styles.end()) {
		return nullptr;
	}

	return &i->second;
}
