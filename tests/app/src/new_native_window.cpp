#include "new_native_window.hpp"

#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/widget/container.hpp>

using namespace std::string_literals;

namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::widget> make_new_native_window_root_widget(utki::shared_ref<ruis::context> c){
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
    return m::column(c,
        {},
        {
            m::text(c,
                {
                    .layout_params{
                        .align = {ruis::align::front, ruis::align::center}
                    }
                },
                U"Native window"s
            ),
            std::move(button)
        }
    );
    // clang-format on
}
