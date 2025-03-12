#include "gradient_window.hpp"

#include <ruis/widget/label/gradient.hpp>
#include <ruis/widget/button/push_button.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::window> make_gradient_window(
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
                },
                .clip = true
            },
            .container_params{
                .layout = ruis::layout::pile
            },
            .title = U"none"s
        },
        {
            m::gradient(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::max}
                    },
                    .gradient_params{
                        .gradient = c.get().loader().load<ruis::res::gradient>("grd_sample"sv)
                    }
                }
            ),
            m::container(c,
                {
                    .layout_params{
                        .dims{ruis::dim::fill, ruis::dim::fill}
                    },
                    .container_params{
                        .layout = ruis::layout::size
                    }
                },
                {
                    m::push_button(c,
                        {
                            .widget_params{
                                .rectangle{
                                    {
                                        (10_pp).get(c.get()),
                                        (20_pp).get(c.get())
                                    },
                                    {}
                                },
                                .enabled = false
                            }
                        },
                        {
                            m::text(c, {}, U"Hello world!"s)
                        }
                    )
                }
            )
        }
    );
    // clang-format on
}
