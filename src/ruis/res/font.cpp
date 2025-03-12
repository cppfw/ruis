/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "font.hpp"

#include <memory>

#include <utki/unicode.hpp>

#include "../context.hpp"
#include "../font/texture_font_provider.hxx"
#include "../util/util.hpp"

using namespace ruis;
using namespace ruis::res;

res::font::font(
	utki::shared_ref<ruis::render::renderer> renderer, //
	const papki::file& file_normal,
	std::unique_ptr<const papki::file> file_bold,
	std::unique_ptr<const papki::file> file_italic,
	std::unique_ptr<const papki::file> file_bold_italic,
	unsigned max_cached
)
{
	// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
	this->fonts[unsigned(style::normal)] =
		std::make_unique<texture_font_provider>(renderer, utki::make_shared<freetype_face>(file_normal), max_cached);

	if (file_bold) {
		// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
		this->fonts[unsigned(style::bold)] =
			std::make_unique<texture_font_provider>(renderer, utki::make_shared<freetype_face>(*file_bold), max_cached);
	}
	if (file_italic) {
		// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
		this->fonts[unsigned(style::italic)] = std::make_unique<texture_font_provider>(
			renderer,
			utki::make_shared<freetype_face>(*file_italic),
			max_cached
		);
	}
	if (file_bold_italic) {
		// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
		this->fonts[unsigned(style::bold_italic)] = std::make_unique<texture_font_provider>(
			std::move(renderer),
			utki::make_shared<freetype_face>(*file_bold_italic),
			max_cached
		);
	}
}

utki::shared_ref<res::font> res::font::load(
	ruis::resource_loader& loader, //
	const tml::forest& desc,
	const papki::file& fi
)
{
	unsigned max_cached = std::numeric_limits<unsigned>::max();

	std::unique_ptr<const papki::file> file_bold;
	std::unique_ptr<const papki::file> file_italic;
	std::unique_ptr<const papki::file> file_bold_italic;

	for (auto& p : desc) {
		if (p.value == "max_cached") {
			max_cached = unsigned(get_property_value(p).to_uint32());
		} else if (p.value == "normal") {
			fi.set_path(get_property_value(p).string);
		} else if (p.value == "bold") {
			// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
			file_bold = fi.spawn(get_property_value(p).string);
		} else if (p.value == "italic") {
			// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
			file_italic = fi.spawn(get_property_value(p).string);
		} else if (p.value == "bold_italic") {
			// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
			file_bold_italic = fi.spawn(get_property_value(p).string);
		}
	}

	return utki::make_shared<font>(
		loader.renderer,
		fi,
		std::move(file_bold),
		std::move(file_italic),
		std::move(file_bold_italic),
		max_cached
	);
}
