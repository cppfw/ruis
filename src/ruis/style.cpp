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

#include "style.hpp"

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
		throw std::invalid_argument(
			utki::cat(
				"style_sheet::parse(desc): unsupported file format version. expected 1, got: ", //
				version
			)
		);
	}

	std::map<std::string, tml::forest, std::less<>> ret;

	for (auto& d : utki::skip_front<1>(desc)) {
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

style_sheet style_sheet::load(const papki::file& fi)
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

style::style(utki::shared_ref<ruis::resource_loader> loader) :
	loader(std::move(loader)),
	cur_style_sheet(utki::make_shared<style_sheet>())
{}

void style::set(utki::shared_ref<style_sheet> ss)
{
	this->cur_style_sheet = std::move(ss);

	std::vector<std::string_view> keys_to_remove;

	// reload cache
	for (auto& pair : this->cache) {
		auto sv = pair.second.lock();
		if (!sv) {
			// the weak reference has expired
			keys_to_remove.emplace_back(pair.first);
			continue;
		}

		const auto* desc = this->cur_style_sheet.get().get(pair.first);
		if (!desc) {
			// there is no such style value id in the new style sheet
			keys_to_remove.emplace_back(pair.first);
			sv->reload(
				{}, // this will set the default value
				this->loader.get()
			);
			continue;
		}

		sv->reload(
			*desc, //
			this->loader.get()
		);
	}

	// remove unused cache entries
	for (const auto& k : keys_to_remove) {
		auto i = this->cache.find(k);
		this->cache.erase(i);
	}
}

std::shared_ptr<style_value_base> style::get_from_cache(std::string_view id)
{
	auto i = this->cache.find(id);
	if (i == this->cache.end()) {
		return nullptr;
	}

	auto p = i->second.lock();
	if (!p) {
		this->cache.erase(i);
	}

	return p;
}

void style::store_to_cache(
	std::string_view id, //
	std::weak_ptr<style_value_base> v
)
{
	ASSERT(!utki::contains(this->cache, id))

	[[maybe_unused]] auto res = this->cache.insert(
		std::make_pair(
			std::string(id), //
			std::move(v)
		)
	);

	ASSERT(res.second) // insert took place
}
