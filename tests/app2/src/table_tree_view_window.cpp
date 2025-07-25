#include "table_tree_view_window.hpp"

#include <ruis/widget/group/window.hpp>

using namespace std::string_view_literals;

namespace m = ruis::make;

utki::shared_ref<ruis::widget> make_table_tree_view_window(
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
				.layout = ruis::layout::column
			},
			.title = c.get().localization.get().get("table_tree_view"sv)
		},
		{}
	);
	// clang-format on
}
