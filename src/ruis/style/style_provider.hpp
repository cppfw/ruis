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
#include <utki/shared_ref.hpp>
#include <utki/string.hpp>
#include <utki/variant.hpp>

#include "../layout/dimension.hpp"
#include "../resource_loader.hpp"
#include "../util/color.hpp"
#include "../util/length.hpp"

#include "style_sheet.hpp"
#include "styled.hpp"

namespace ruis {

namespace res {
class font;
} // namespace res

class style_provider
{
	template <typename styled_value_type>
	friend class styled;

	mutable utki::enum_array<std::weak_ptr<internal::style_value_base>, style> standard_cache;
	mutable std::map<std::string, std::weak_ptr<internal::style_value_base>, std::less<>> user_cache;

	std::shared_ptr<const internal::style_value_base> get_from_cache(style id) const;
	std::shared_ptr<const internal::style_value_base> get_from_cache(std::string_view id) const;

	void store_to_cache(
		style id, //
		std::weak_ptr<internal::style_value_base> v
	) const;

	void store_to_cache(
		std::string_view id, //
		std::weak_ptr<internal::style_value_base> v
	) const;

public:
	const utki::shared_ref<ruis::resource_loader> res_loader;

private:
	utki::shared_ref<ruis::style_sheet> cur_style_sheet;

	template <typename value_type>
	styled<value_type> get(style id) const
	{
		if (auto svb = this->get_from_cache(id)) {
			if (auto sv = std::dynamic_pointer_cast<const typename styled<value_type>::style_value>(svb)) {
				return {utki::shared_ref<const typename styled<value_type>::style_value>(std::move(sv))};
			}
			throw std::invalid_argument("style::get(id): requested value_type does not match the one stored in cache");
		}
		const auto& desc = this->cur_style_sheet.get().get(id);

		if (desc.empty()) {
			throw std::invalid_argument(
				utki::cat("style_provider::get(style): no style value with id ", unsigned(id), " in the style sheet")
			);
		}

		auto ret = utki::make_shared<typename styled<value_type>::style_value>(
			desc, //
			this->res_loader.get()
		);
		this->store_to_cache(
			id, //
			ret.to_shared_ptr()
		);
		return {ret};
	}

public:
	style_provider(utki::shared_ref<ruis::resource_loader> loader);

	void set(utki::shared_ref<style_sheet> ss);

	template <typename value_type>
	styled<value_type> get(std::string_view id) const
	{
		if (auto svb = this->get_from_cache(id)) {
			if (auto sv = std::dynamic_pointer_cast<const typename styled<value_type>::style_value>(svb)) {
				return {utki::shared_ref<const typename styled<value_type>::style_value>(std::move(sv))};
			}
			throw std::invalid_argument("style::get(id): requested value_type does not match the one stored in cache");
		}

		const auto* desc = this->cur_style_sheet.get().get(id);
		if (!desc) {
			return typename styled<value_type>::actual_value_type();
		}

		auto ret = utki::make_shared<typename styled<value_type>::style_value>(
			*desc, //
			this->res_loader.get()
		);
		this->store_to_cache(
			id, //
			ret.to_shared_ptr()
		);
		return {ret};
	}

	// ===================================
	// ====== standard style values ======

	styled<color> get_color_background() const;
	styled<color> get_color_middleground() const;
	styled<color> get_color_foreground() const;
	styled<color> get_color_text_normal() const;
	styled<color> get_color_text_highlight() const;
	styled<color> get_color_highlight() const;
	styled<color> get_color_cursor() const;

	styled<layout::dimension> get_dim_indent_tree_view_item() const;

	styled<length> get_font_size_normal() const;
	styled<res::font> get_font_face_normal() const;
};

} // namespace ruis
