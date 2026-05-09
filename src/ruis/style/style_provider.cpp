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

#include "style_provider.hpp"

#include "../res/font.hpp"

using namespace std::string_view_literals;
using namespace ruis;

style_provider::style_provider(utki::shared_ref<ruis::resource_loader> loader) :
	res_loader(std::move(loader)),
	cur_style_sheet(utki::make_shared<style_sheet>())
{}

void style_provider::set(utki::shared_ref<style_sheet> ss)
{
	this->cur_style_sheet = std::move(ss);

	// ==== reload standard cache ====
	for (auto [v, id] : this->standard_cache.zip_with_enum()) {
		auto sv = v.lock();
		if (!sv) {
			continue;
		}

		sv->reload(
			this->cur_style_sheet.get().get(id), //
			this->res_loader.get()
		);
	}

	// ==== reload user cache ====
	std::vector<std::string_view> keys_to_remove;

	for (auto& pair : this->user_cache) {
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
				this->res_loader.get()
			);
			continue;
		}

		sv->reload(
			*desc, //
			this->res_loader.get()
		);
	}

	// remove unused cache entries
	for (const auto& k : keys_to_remove) {
		auto i = this->user_cache.find(k);
		this->user_cache.erase(i);
	}
}

std::shared_ptr<const style_provider::style_value_base> style_provider::get_from_cache(style id) const
{
	auto& w = this->standard_cache[id];
	auto p = w.lock();
	if (!p) {
		return nullptr;
	}

	return p;
}

std::shared_ptr<const style_provider::style_value_base> style_provider::get_from_cache(std::string_view id) const
{
	auto i = this->user_cache.find(id);
	if (i == this->user_cache.end()) {
		return nullptr;
	}

	auto p = i->second.lock();
	if (!p) {
		this->user_cache.erase(i);
	}

	return p;
}

void style_provider::store_to_cache(
	style id, //
	std::weak_ptr<style_value_base> v
) const
{
	utki::assert(this->standard_cache[id].expired());

	this->standard_cache[id] = std::move(v);
}

void style_provider::store_to_cache(
	std::string_view id, //
	std::weak_ptr<style_value_base> v
) const
{
	utki::assert(!utki::contains(this->user_cache, id));

	[[maybe_unused]] auto res = this->user_cache.insert(std::make_pair(
		std::string(id), //
		std::move(v)
	));

	// insert took place
	utki::assert(res.second);
}

styled<color> style_provider::get_color_background() const
{
	return this->get<color>(style::color_background);
}

styled<color> style_provider::get_color_middleground() const
{
	return this->get<color>(style::color_middleground);
}

styled<color> style_provider::get_color_foreground() const
{
	return this->get<color>(style::color_foreground);
}

styled<color> style_provider::get_color_text_normal() const
{
	return this->get<color>(style::color_text_normal);
}

styled<color> style_provider::get_color_highlight() const
{
	return this->get<color>(style::color_highlight);
}

styled<color> style_provider::get_color_cursor() const
{
	return this->get<color>(style::color_cursor);
}

styled<layout::dimension> style_provider::get_dim_indent_tree_view_item() const
{
	return this->get<layout::dimension>(style::dim_tree_view_item_indent);
}

styled<length> style_provider::get_font_size_normal() const
{
	return this->get<length>(style::font_size_normal);
}

styled<res::font> style_provider::get_font_face_normal() const
{
	return this->get<res::font>(style::font_face_normal);
}
