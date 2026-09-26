#include "main_page.hpp"

#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/text.hpp>

#include "pile_page.hpp"

using namespace std::string_literals;

namespace m {
using namespace ruis::make;
using namespace ::make;
} // namespace m

utki::shared_ref<ruis::page> make_main_page(const utki::shared_ref<ruis::context>& c)
{
	// clang-format off
    return m::pile_page(c,
        ruis::widget::parameters{},
        {
            m::column(c,
                {
                    .layout_params{
                        .dims = {ruis::dim::fill, ruis::dim::fill}
                    }
                },
                {
                    m::push_button(c,
                        {
                            .layout_params{
                                .dims = {ruis::dim::fill, ruis::dim::min}
                            },
                            .widget{
                                .id = "cube_button"s
                            }
                        },
                        {
                            m::text(c,
                                {},
                                U"Cube!"s
                            )
                        }
                    ),
                    m::push_button(c,
                        {
                            .layout_params{
                                .dims = {ruis::dim::fill, ruis::dim::min}
                            },
                            .widget{
                                .id = "stuff_button"s
                            }
                        },
                        {
                            m::text(c,
                                {},
                                U"Stuff!"s
                            )
                        }
                    ),
                    m::push_button(c,
                        {
                            .layout_params{
                                .dims = {ruis::dim::fill, ruis::dim::min}
                            },
                            .widget{
                                .id = "close_button"s
                            }
                        },
                        {
                            m::text(c,
                                {},
                                U"Close!"s
                            )
                        }
                    )
                }
            )
        }
    );
	// clang-format on
}
