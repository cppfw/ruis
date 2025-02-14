#include "sliders_window.hpp"

#include <ruis/widget/label/image.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::window> make_sliders_window(
    utki::shared_ref<ruis::context> c, //
    ruis::vec2_length pos
)
{
    // clang-format off
    return m::window(c,
        {
            .widget_params{
                .rectangle{
                    {
                        pos.x().get(c.get()),
                        pos.y().get(c.get())
                    },
                    {
                        ruis::length::make_pp(300).get(c.get()),
                        ruis::length::make_pp(200).get(c.get())
                    }
                }
            },
            .container_params{
                .layout = ruis::layout::row
            },
            .title = U"Sliders"s
        },
        {
            m::image(c,
                {
                    .layout_params{
                        .dims{ruis::dim::min, ruis::dim::max}
                    },
                    .image_params{
                        .img = c.get().loader.load<ruis::res::image>("img_camera"sv),
                        .keep_aspect_ratio = true,
                    }
                }
            ),
            m::scroll_bar(c,
                {
                    .layout_params{
                        .dims{ruis::dim::fill, 40_pp},
                        .weight = 1
                    },
                    .oriented_params{
                        .vertical = false
                    }
                }
            ),
            m::scroll_bar(c,
                {
                    .layout_params{
                        .dims{40_pp, ruis::dim::fill}
                    },
                    .widget_params{
                        .id = "vertical slider"s
                    },
                    .oriented_params{
                        .vertical = true
                    }
                }
            )
        }
    );
    // clang-format on
}
