#include "gui.hpp"

#include <ruis/widgets/container.hpp>

namespace m = ruis::make;
using lp = ruis::lp;

utki::shared_ref<ruis::widget> make_gui(utki::shared_ref<ruis::context> c)
{
	return m::container(
		c,
		{.container_params = {.layout = ruis::layout::row}},
		{

		}
	);
}
