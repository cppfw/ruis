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

#include "table_list_window.hpp"

#include <ruis/widget/group/table_list.hpp>
#include <ruis/widget/group/window.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>

#include "table_tree_view_window.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

namespace {
const tml::forest list_data = tml::read(R"qwertyuiop(
		""{"row 0" "0123" "321sadf0"}
		""{"row 1" "12345" "54321asdfas"}
		""{"row 2" "2345" "54321asdf"}
		""{"row 3" "3452434" "5432sdfas1"}
		""{"row 4" "4512124" "5432asdf1"}
		""{"row 5" "54123" "5432dff1"}
		""{"row 6" "6 324123" "54asdf321"}
		""{"row 7" "7 546 45" "543sdafas21"}
		""{"row 8" "8 3453" "5432asdf1"}
		""{"row 9" "9 3453245" "543asdf21"}
		""{"row 10" "10 345222345" "543gfgsdf21"}
		""{"row 11" "11 35432345" "5432sgdfgsdfgs1"}
		""{"row 12" "12 35324345" "sdfgsdfg54321"}
		""{"row 13" "13 35432445" "5432sdfg1"}
		""{"row 14" "14 345 345" "5432gsfd1"}
		""{"row 15" "15 3 45234" "543sdsdf21"}
		""{"row 16" "16 543345" "5432gsdf1"}
		""{"row 17" "17 5234 23534" "5432dfg1"}
		""{"row 18" "18 4534 5324" "543dfg21"}
		""{"row 19" "19 3453 45 " "5432sgdfsdfg1"}
		""{"row 20" "20 45 2345 342" "543sdfg21"}
	)qwertyuiop");
} // namespace

namespace {
utki::shared_ref<ruis::widget> make_table_list_header(
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

utki::shared_ref<ruis::widget> make_table_list_window(
	const utki::shared_ref<ruis::context>& c, //
	ruis::vec2_length pos
)
{
	// clang-format off
	auto table_list = m::table_list(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill},
				.weight = 1
			},
			.table_list_params{
				.column_headers = {
					make_table_list_header(c, U"col 1"s),
					make_table_list_header(c, U"col 2"s),
					make_table_list_header(c, U"col 3"s)
				},
				.provider = [&](){
					class provider : public ruis::table_list::provider{
					public:
						provider(utki::shared_ref<ruis::context> context) :
							ruis::table_list::provider(std::move(context))
						{}

						size_t count() const noexcept override{
							return list_data.size();
						}

						ruis::widget_list get_row_widgets(size_t index) override{
							ruis::widget_list ret;

							for(auto i = 0; i != 3; ++i){
								ret.emplace_back(
									m::text(
										this->context,//
										{.widget_params{.clip = true}},
										utki::to_utf32(list_data[index].children[i].value.string)
									)
								);
							}
							return ret;
						}
					};
					return utki::make_shared<provider>(c);
				}()
			}
		}
	);

	auto scroll_bar = m::scroll_bar(c,
		{
			.layout_params{
				.dims = {ruis::dim::min, ruis::dim::fill}
			}
		}
	);

	scroll_bar.get().fraction_change_handler =
		[table_list_weak = utki::make_weak(table_list)]
		(ruis::fraction_widget& sb)
	{
		if(auto l = table_list_weak.lock()){
			l->set_scroll_factor(
				sb.get_fraction(),
				false // no notify
			);
		}
	};

	table_list.get().scroll_change_handler =
		[scroll_bar_weak = utki::make_weak(scroll_bar)]
		(ruis::table_list& tl)
	{
		if(auto sb = scroll_bar_weak.lock()){
			sb->set_fraction(
				tl.get_scroll_factor(),
				false // no notify
			);
			sb->set_band_fraction(tl.get_scroll_band());
		}
	};
 
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
			.title = c.get().localization.get().get("table_list"sv)
		},
		{
			std::move(table_list),
			std::move(scroll_bar)
		}
	);
	// clang-format on
}
