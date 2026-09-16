#include "spinning_cube_window.hpp"

#include <ruis/widget/label/image.hpp>
#include <ruis/widget/proxy/click_proxy.hpp>

#include "cube_widget.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m{
using namespace ruis::make;
using namespace ::make;
}

utki::shared_ref<ruis::window> make_spinning_cube_window(
    const utki::shared_ref<ruis::context>& c, //
    ruis::vec2_length pos
)
{
    // clang-format off
    return m::window(c,
        {
            .widget{
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
            .params{
                .layout = ruis::layout::pile
            },
            .title = U"Spinning cube"s
        },
        {
            m::rectangle(c,
                {
                    .layout{
                        .dims{ruis::dim::fill, ruis::dim::fill}
                    },
                    .widget{
                        .id = "cube_bg_color"s
                    }
                }
            ),
            m::image(c,
                {
                    .params{
                        .specific{
                            .source = c.get().loader().load<ruis::res::image>("img_lattice"sv)
                        }
                    }
                }
            ),
            m::click_proxy(c,
                {
                    .layout{
                        .dims{ruis::dim::fill, ruis::dim::fill}
                    },
                    .widget{
                        .id = "cube_click_proxy"s
                    }
                }
            ),
            m::cube_widget(c,
                {
                    .layout{
                        .dims{ruis::dim::max, ruis::dim::max}
                    },
                    .widget{
                        .id = "cube_widget"s,
                        .cache = true,
                        .depth = true
                    }
                }
            )
        }
    );
    // clang-format on
}
