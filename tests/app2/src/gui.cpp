#include "gui.hpp"

#include <ruis/widgets/container.hpp>
#include <ruis/widgets/group/window.hpp>

namespace m = ruis::make;
using lp = ruis::lp;

utki::shared_ref<ruis::widget> make_gui(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return m::container(
		c,
		{
			.container_params = {
				.layout = ruis::layout::trivial
			}
		},
		{
			m::window(c,
				{
					.widget_params = {
						.rectangle = {10, 20, 300, 200}
					}
				},
				{}
			)
		}
	);
	// clang-format on
}
