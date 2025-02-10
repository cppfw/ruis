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

#pragma once

#include <unordered_map>

#include "../button/tab.hpp"
#include "../button/tab_group.hpp"

#include "book.hpp"

namespace ruis {

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via widget")
class tabbed_book :
	virtual public ruis::widget, //
	private ruis::container
{
	ruis::tab_group& tab_group;
	ruis::book& book;

	struct tab_page_pair {
		ruis::tab* tab;
		ruis::page* page;
	};

	std::vector<tab_page_pair> tab_page_pairs;

	decltype(tab_page_pairs)::iterator find_pair(const ruis::tab& t);
	decltype(tab_page_pairs)::iterator find_pair(const ruis::page& p);

	void activate_another_tab(tab& t);

public:
	tabbed_book(const utki::shared_ref<ruis::context>& context, const tml::forest& desc);

	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
	};

	tabbed_book(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	void add(const utki::shared_ref<tab>& tab, const utki::shared_ref<ruis::page>& page);

	const ruis::book& get_book() const noexcept
	{
		return this->book;
	}

	utki::shared_ref<page> tear_out(tab& t);

	using ruis::container::render;
	using ruis::container::on_lay_out;
	using ruis::container::on_mouse_move;
	using ruis::container::on_mouse_button;
	using ruis::container::on_key;
	using ruis::container::on_focus_change;
	using ruis::container::on_hovered_change;
	using ruis::container::on_resize;
	using ruis::container::on_parent_change;
	using ruis::container::measure;
	using ruis::container::on_enabled_change;
};

namespace make {
inline utki::shared_ref<ruis::tabbed_book> tabbed_book(
	utki::shared_ref<ruis::context> context, //
	ruis::tabbed_book::all_parameters params
)
{
	return utki::make_shared<ruis::tabbed_book>(
		std::move(context), //
		std::move(params)
	);
}
} // namespace make

} // namespace ruis
