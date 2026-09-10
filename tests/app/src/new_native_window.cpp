#include "new_native_window.hpp"

#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/label/nine_patch.hpp>
#include <ruis/widget/input/text_field.hpp>
#include <ruisapp/application.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::widget> make_new_native_window_root_widget(
    const utki::shared_ref<ruis::context>& c, //
    ruisapp::window& win
){
    // clang-format off
    auto button = m::push_button(c,
        {
            .layout_params{
                .dims{ruis::dim::fill, ruis::dim::fill},
                .weight = 1
            }
        },
        {
            m::text(c, {}, U"the button"s)
        }
    );
    // clang-format on

    button.get().click_handler = [](ruis::push_button& b){
        utki::logcat("the button clicked", '\n');
    };

    // clang-format off
    auto close_button = m::push_button(c,
        {},
        {
            m::text(c, {}, U"X"s)
        }
    );
    // clang-format on

    close_button.get().click_handler = [&win](auto& b){
        ruisapp::inst().destroy_window(win);
    };

    // clang-format off
    return m::column(c,
        {},
        {
            m::row(c,
                {
                    .layout_params{
                        .align = {ruis::align::front, ruis::align::center}
                    }
                },
                {
                    m::text(c, {}, U"Native window"s),
                    std::move(close_button)
                }
            ),
            m::text_field(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::min}
                    },
                    .widget_params{
                        .id = "text_input"s
                    },
                    .text_input_params{
                        .color_params{
                            .color = c.get().style().get_color_text()
                        }
                    }
                },
                U"Hello Wrodl!!!"s
            ),
            std::move(button)
        }
    );
    // clang-format on
}
