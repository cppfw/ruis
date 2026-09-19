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
	} else if (name == "color_primary"sv) {
		return style::color_primary;
	} else if (name == "color_panel"sv) {
		return style::color_panel;
	} else if (name == "color_dimmed"sv) {
		return style::color_dimmed;
	} else if (name == "color_text"sv) {
		return style::color_text;
	} else if (name == "color_text_secondary"sv) {
		return style::color_text_secondary;
	} else if (name == "color_secondary"sv) {
		return style::color_secondary;
	} else if (name == "color_highlight"sv) {
		return style::color_highlight;
	} else if (name == "color_special"sv) {
		return style::color_special;
	} else if (name == "len_indent"sv) {
		return style::len_indent;
	} else if (name == "len_gap_small"sv) {
		return style::len_gap_small;
	} else if (name == "len_gap"sv) {
		return style::len_gap;
	} else if (name == "len_gap_big"sv) {
		return style::len_gap_big;
	} else if (name == "len_border"sv) {
		return style::len_border;
	} else if (name == "len_button_padding"sv) {
		return style::len_button_padding;
	} else if (name == "len_dialog_margin"sv) {
		return style::len_dialog_margin;
	} else if (name == "len_dialog_padding"sv) {
		return style::len_dialog_padding;
	} else if (name == "font_size_text"sv) {
		return style::font_size_text;
	} else if (name == "font_size_title"sv) {
		return style::font_size_title;
	} else if (name == "font_face_text"sv) {
		return style::font_face_text;
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
				if (s.children.empty()) {
					throw std::invalid_argument(
						utki::cat("style_sheet::parse(desc): empty style value for: ", s.value.string)
					);
				}
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

namespace {

const utki::enum_array<tml::forest, style>& default_style_forests()
{
	static const auto defaults = []() {
		utki::enum_array<tml::forest, style> d{};
		d[style::color_background] = tml::read("0xff101010");
		d[style::color_panel] = tml::read("0xff424242");
		d[style::color_special] = tml::read("0xffff8080");
		d[style::color_primary] = tml::read("0xff505050");
		d[style::color_secondary] = tml::read("0xff303030");
		d[style::color_highlight] = tml::read("0xffad9869");
		d[style::color_dimmed] = tml::read("0xb0000000");
		d[style::color_text] = tml::read("0xffffffff");
		d[style::color_text_secondary] = tml::read("0xffa0a0a0");
		d[style::len_indent] = tml::read("17pp");
		d[style::len_gap_small] = tml::read("4pp");
		d[style::len_gap] = tml::read("8pp");
		d[style::len_gap_big] = tml::read("16pp");
		d[style::len_border] = tml::read("1pp");
		d[style::len_button_padding] = tml::read("5pp");
		d[style::len_dialog_margin] = tml::read("30pp");
		d[style::len_dialog_padding] = tml::read("20pp");
		d[style::font_size_text] = tml::read("14pp");
		d[style::font_size_title] = tml::read("22pp");
		d[style::font_face_text] = tml::read("ruis_fnt_normal");
		return d;
	}();
	return defaults;
}

} // namespace

const tml::forest& style_sheet::get(style style_id) const noexcept
{
	const auto& f = this->standard_styles[style_id];
	if (!f.empty()) {
		return f;
	}
	return default_style_forests()[style_id];
}

const tml::forest* style_sheet::get(std::string_view style_id) const noexcept
{
	auto i = this->user_styles.find(style_id);
	if (i == this->user_styles.end()) {
		return nullptr;
	}

	return &i->second;
}
