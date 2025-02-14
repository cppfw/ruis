#include "text_input_window.hpp"

#include <ruis/widget/button/tab_group.hpp>
#include <ruis/widget/button/tab.hpp>
#include <ruis/widget/group/collapse_area.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::window> make_text_input_window(
    utki::shared_ref<ruis::context> c,
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
                .layout = ruis::layout::column
            },
            .title = U"TextInput"s
        },
        {
            m::tab_group(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::min}
                    }
                },
                {
                    m::tab(c,
                        {
                            .widget_params{
                                .id = "tab1"s
                            }
                        },
                        {
                            m::text(c, {}, U"tab1"s)
                        }
                    ),
                    m::tab(c, {}, {m::text(c, {}, U"tab2"s)}),
                    m::tab(c, {}, {m::text(c, {}, U"tab 3"s)}),
                    m::tab(c, {}, {m::text(c, {}, U"tab four"s)})
                }
            ),
            m::nine_patch(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::min}
                    },
                    .nine_patch_params{
                        .nine_patch = c.get().loader.load<ruis::res::nine_patch>("ruis_npt_frame"s)
                    }
                },
                {
                    m::collapse_area(c,
                        {},
                        {
                            // TODO:
                        }
                    )
                }
            )
        }
    );
    // clang-format on
}
