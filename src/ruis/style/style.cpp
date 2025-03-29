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

std::shared_ptr<style::style_value_base> style::get_from_cache(std::string_view id)
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

// styled<color> style::get_color_background() const
// {
// 	return this->get<color>("color_background"sv);
// }

// styled<color> style::get_color_middleground() const
// {
// 	return this->get<color>("color_middleground"sv);
// }

// styled<color> style::get_color_foreground() const
// {
// 	return this->get<color>("color_foreground"sv);
// }
