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

#include <unordered_map>

#include "../button/choice_button.hpp"
#include "../button/tab_group.hpp"

#include "book.hpp"

namespace ruis {

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via widget")
class tabbed_book :
	virtual public ruis::widget, //
	private ruis::container
{
	// NOLINTNEXTLINE(clang-analyzer-webkit.NoUncountedMemberChecker, "false-positive")
	ruis::choice_group& choice_group;

	// NOLINTNEXTLINE(clang-analyzer-webkit.NoUncountedMemberChecker, "false-positive")
	ruis::book& book;

	std::vector< //
		std::pair<
			utki::shared_ref<choice_button>, //
			utki::shared_ref<page> //
			> //
		>
		tab_page_pairs;

	decltype(tab_page_pairs)::iterator find_pair(const ruis::choice_button& t);
	decltype(tab_page_pairs)::iterator find_pair(const ruis::page& p);

	void activate_another_tab(choice_button& t);

	void set_tab_pressed_change_handler(typename decltype(tab_page_pairs)::value_type& pair);

public:
	struct parameters {
		std::function< //
			utki::shared_ref<ruis::book>(
				utki::shared_ref<ruis::context>, //
				std::vector<utki::shared_ref<page>>
			) //
		> book_factory;

		std::function< //
			utki::shared_ref<ruis::choice_group>(
				utki::shared_ref<ruis::context>, //
				widget_list
			) //
			>
			choice_group_factory;

		bool reverse_order = false;
	};

	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		parameters tabbed_book_params;
	};

	using pages_list_type = std::vector< //
		std::pair<
			utki::shared_ref<choice_button>, //
			utki::shared_ref<page> //
			> //
		>;

	tabbed_book(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		pages_list_type pages
	);

private:
	tabbed_book(
		utki::shared_ref<ruis::context> context, //
		all_parameters& params,
		pages_list_type& pages,
		utki::shared_ref<ruis::choice_group> choice_group,
		utki::shared_ref<ruis::book> book
	);

public:
	void add(
		utki::shared_ref<choice_button> tab, //
		utki::shared_ref<ruis::page> page
	);

	const ruis::book& get_book() const noexcept
	{
		return this->book;
	}

	utki::shared_ref<page> tear_out(choice_button& t);

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
utki::shared_ref<ruis::tabbed_book> tabbed_book(
	utki::shared_ref<ruis::context> context, //
	ruis::tabbed_book::all_parameters params,
	std::vector< //
		std::pair<
			utki::shared_ref<ruis::choice_button>, //
			utki::shared_ref<ruis::page> //
			> //
		> //
		pages = {}
);
} // namespace make

} // namespace ruis
