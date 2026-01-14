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

#include "table_tree_view_window.hpp"

#include <ruis/widget/group/table_tree_view.hpp>
#include <ruis/widget/group/window.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

namespace {
class table_tree_view_provider : public ruis::table_tree_view::provider
{
	tml::forest root = tml::read(R"qwertyuiop(
		root1.123567890.4567890123.789123456.012345667{
			subroot1.1.2.3.4.5{
				subsubroot1.1.2
				subsubroot2.3.4
				subsubroot3.5.6
				subsubroot4.7.8
			}
			subroot2.q.w
			subroot3.a.s{
				subsubroot0.z.x
				subsubroot1.o.p{
					subsubsubroot1.k.l
					subsubsubroot2.n.m
				}
				subsubroot2.a.z
			}
		}
		root22.s.x{
			subsubroot1.d.c
			subsubroot2.f.v{
				trololo.g.b
				"hello world!.h.n"
			}
		}
		root333.j.m
		root4444.h.j
	)qwertyuiop");

public:
	table_tree_view_provider(utki::shared_ref<ruis::context> context) :
		ruis::table_tree_view::provider(std::move(context))
	{}

	size_t count(utki::span<const size_t> index) const noexcept override
	{
		if (index.empty()) {
			return this->root.size();
		}

		auto tr = utki::make_traversal(this->root);
		ASSERT(
			tr.is_valid(index), //
			[&](auto& o) {
				o << "index.size() = " << index.size();
			}
		)
		try {
			auto i = tr.make_iterator(index);
			return i->children.size();
		} catch (std::out_of_range&) {
			// should never happen
			utki::assert(false, SL);
			return 0;
		}
	}

	ruis::widget_list get_row_widgets(utki::span<const size_t> index) override
	{
		auto tr = utki::make_traversal(this->root);
		ASSERT(tr.is_valid(index))
		auto i = tr.make_iterator(index);

		auto cell_values = utki::split(i->value.string, '.');

		ruis::widget_list wl;
		for (auto& c : cell_values) {
			wl.emplace_back(ruis::make::text(
				this->context, //
				{.widget_params{.clip = true}},
				utki::to_utf32(c)
			));
		}

		return wl;
	}
};
} // namespace

namespace {
utki::shared_ref<ruis::widget> make_table_tree_view_header(
	const utki::shared_ref<ruis::context>& c, //
	std::u32string text
)
{
	// clang-format off
	return m::rectangle(c,
		{
			.padding_params{
				.borders = {5_pp}
			},
			.color_params{
				.color = 0xff006060
			},
			.rectangle_params{
				.corner_radii = {3_pp}
			}
		},
		{
			m::text(c,
				{},
				std::move(text)
			)
		}
	);
	// clang-format on
}
} // namespace

utki::shared_ref<ruis::widget> make_table_tree_view_window(
	const utki::shared_ref<ruis::context>& c, //
	ruis::vec2_length pos
)
{
	// clang-format off
	auto table_tree_view = m::table_tree_view(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::max},
				.weight = 1
			},
			.table_tree_view_params{
				.column_headers = {
					make_table_tree_view_header(c, U"col 1"s),
					make_table_tree_view_header(c, U"col 2"s),
					make_table_tree_view_header(c, U"col 3"s)
				},
				.provider = utki::make_shared<table_tree_view_provider>(c)
			}
		}
	);
	// clang-format on

	// clang-format off
	auto scroll_bar = m::scroll_bar(c,
		{
			.layout_params{
				.dims = {ruis::dim::min, ruis::dim::fill}
			}
		}
	);
	// clang-format on

	scroll_bar.get().fraction_change_handler = [table_tree_view_weak =
													utki::make_weak(table_tree_view)](ruis::fraction_widget& sb) {
		if (auto tv = table_tree_view_weak.lock()) {
			tv->set_scroll_factor(
				sb.get_fraction(),
				false // no notify
			);
		}
	};

	table_tree_view.get().scroll_change_handler = [scroll_bar_weak =
													   utki::make_weak(scroll_bar)](ruis::table_list& tl) {
		if (auto sb = scroll_bar_weak.lock()) {
			sb->set_fraction(
				tl.get_scroll_factor(),
				false // no notify
			);
			sb->set_band_fraction(tl.get_scroll_band());
		}
	};

	// clang-format off
	return m::window(c,
		{
			.widget_params = {
				.rectangle = {
					{
						pos.x().get(c),
						pos.y().get(c)
					},
					{
						ruis::length::make_pp(300).get(c),
						ruis::length::make_pp(200).get(c)
					}
				}
			},
			.container_params = {
				.layout = ruis::layout::row
			},
			.title = c.get().localization.get().get("table_tree_view"sv)
		},
		{
			table_tree_view,
			scroll_bar
		}
	);
	// clang-format on
}
