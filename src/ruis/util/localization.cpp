/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "localization.hpp"

#include <utki/unicode.hpp>

using namespace std::string_view_literals;

using namespace ruis;

wording::wording(
	std::shared_ptr<vocabulary_type> vocabulary, //
	std::string_view id
) :
	vocabulary(std::move(vocabulary)),
	iter([&]() {
		ASSERT(this->vocabulary)
		auto i = this->vocabulary->find(id);
		if (i == this->vocabulary->end()) {
			throw std::invalid_argument(utki::cat("could not find localized string with id: ", id));
		}
		return i;
	}())
{}

wording localization::get(std::string_view id)
{
	return {this->vocabulary.to_shared_ptr(), id};
}

namespace {
std::u32string read_localization_string(const tml::forest& desc)
{
	for (const auto& d : desc) {
		if (d.value.string == "str"sv) {
			if (d.children.empty()) {
				return {};
			}

			return utki::to_utf32(d.children.front().value.string);
		}
	}
	return {};
}
} // namespace

utki::shared_ref<wording::vocabulary_type> localization::read_localization_vocabulary(const tml::forest& desc)
{
	auto vocabulary = utki::make_shared<wording::vocabulary_type>();
	auto& voc = vocabulary.get();

	for (const auto& d : desc) {
		if (d.value == "vocabulary"sv) {
			for (const auto& v : d.children) {
				voc[v.value.string] = read_localization_string(v.children);
			}
		}
	}

	return vocabulary;
}

localization::localization(const tml::forest& desc) :
	vocabulary(read_localization_vocabulary(desc))
{}

wording localization::reload(wording&& w){
	return this->get(w.id());
}
