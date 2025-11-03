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

#include "style_sheet.hpp"

using namespace std::string_view_literals;
using namespace ruis;

style_sheet::style_sheet(tml::forest desc) :
	id_to_description_map(parse(std::move(desc)))
{}

std::map<std::string, tml::forest, std::less<>> style_sheet::parse(tml::forest desc)
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

	std::map<std::string, tml::forest, std::less<>> ret;

	for (auto& d : utki::skip_front<1>(desc)) {
		// TODO: why "ruis" and "user" are stored into same place?
		if (d.value.string == "ruis"sv || d.value.string == "user"sv) {
			for (auto& s : d.children) {
				ret.insert_or_assign(
					std::move(s.value.string), //
					std::move(s.children)
				);
			}
		}
	}

	return ret;
}

style_sheet style_sheet::load(const fsif::file& fi)
{
	return {tml::read(fi)};
}

const tml::forest* style_sheet::get(std::string_view style_id) const noexcept
{
	auto i = this->id_to_description_map.find(style_id);
	if (i == this->id_to_description_map.end()) {
		return nullptr;
	}

	return &i->second;
}
