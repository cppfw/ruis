#include "table_list_window.hpp"

#include <ruis/widget/group/table_list.hpp>
#include <ruis/widget/group/window.hpp>
#include <ruis/widget/label/rectangle.hpp>

#include "table_tree_view_window.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

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
				.layout = ruis::layout::pile
			},
			.title = c.get().localization.get().get("table_list"sv)
		},
		{
			m::table_list(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, ruis::dim::fill}
					},
					.table_list_params{
						.column_headers = {
							make_table_list_header(c, U"col 1"s),
							make_table_list_header(c, U"col 2"s),
							make_table_list_header(c, U"col 3"s)
						}
					}
				}
			)
		}
	);
	// clang-format on
}
