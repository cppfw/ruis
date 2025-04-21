#include "rectangles_window.hpp"

#include <ruis/widget/group/window.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/label/text.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

utki::shared_ref<ruis::widget> make_rectangles_window(
	const utki::shared_ref<ruis::context>& c, //
	ruis::rect rect
)
{
	// clang-format off
    return m::window(c,
        {
            .widget_params = {
				.rectangle = rect
			},
			.container_params = {
				.layout = ruis::layout::column
			},
			.title = c.get().localization.get().get("rectangles"sv)
        },
        {
            m::rectangle(c,
                {
                    .color_params{
                        .color = 0xff800000
                    }
                },
                {
                    m::text(c, {}, U"no borders"s)
                }
            ),
            m::gap(c,
                {
                    .layout_params{
                        .dims = {0_px, 5_pp}
                    }
                }
            ),
            m::rectangle(c,
                {
                    .frame_params{
                        .borders{
                            5_pp, // left
                            10_pp, // top
                            15_pp, // right
                            20_pp // bottom
                        }
                    },
                    .color_params{
                        .color = 0xff800000
                    }
                },
                {
                    m::text(c, {}, U"borders"s)
                }
            ),
            m::gap(c,
                {
                    .layout_params{
                        .dims = {0_px, 5_pp}
                    }
                }
            ),
            m::rectangle(c,
                {
                    .frame_params{
                        .borders{
                            5_pp, // left
                            10_pp, // top
                            15_pp, // right
                            20_pp // bottom
                        }
                    },
                    .color_params{
                        .color = 0xff800000
                    },
                    .rect_params{
                        .rounded_corners = {true, true, true, true}
                    }
                },
                {
                    m::text(c, {}, U"rounded corners"s)
                }
            ),
        }
    );
	// clang-format on
}
