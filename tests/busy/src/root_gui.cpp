#include "root_gui.hpp"

#include <ruis/widget/label/busy.hpp>
#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/text.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace m{
    using namespace ruis::make;
}

utki::shared_ref<ruis::widget> make_root_gui(utki::shared_ref<ruis::context> c){
    // clang-format off
    return m::pile(c,
        {},
        {
            m::column(c,
                {},
                {
                    m::busy(c,
                        {
                            .widget_params{
                                .id = "busy_spinner"s,
                                .visible = false
                            }
                        }
                    ),
                    m::push_button(c,
                        {
                            .widget_params{
                                .id = "busy_toggle_button"s
                            }
                        },
                        {
                            m::text(c,
                                {},
                                U"toggle busy spinner"s
                            )
                        }
                    ),
                    m::busy(c,
                        {
                            .widget_params{
                                .id = "busy_spinner2"s
                            }
                        }
                    ),
                    m::push_button(c,
                        {
                            .widget_params{
                                .id = "refresh_toggle_button"s
                            }
                        },
                        {
                            m::refresh(c,
                                {
                                    .widget_params{
                                        .id = "refresh_spinner"s
                                    }
                                }
                            )
                        }
                    ),
                    m::push_button(c,
                        {
                            .widget_params{
                                .id = "refresh_disable_button"s
                            }
                        },
                        {
                            m::refresh(c, {})
                        }
                    )
                }
            )
        }
    );
    // clang-format on
}