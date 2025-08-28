#include "text_input_window.hpp"

#include <ruis/widget/button/tab_group.hpp>
#include <ruis/widget/button/tab.hpp>
#include <ruis/widget/group/collapse_area.hpp>
#include <ruis/widget/button/impl/image_push_button.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/input/text_input_line.hpp>
#include <ruis/default_style.hpp>
#include <ruisapp/application.hpp>

#include "new_native_window.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m{
using namespace ruis::make;
}

namespace{
utki::shared_ref<ruis::push_button> make_push_button(
    utki::shared_ref<ruis::context> c, //
    std::u32string text
)
{
    // clang-format off
    return m::push_button(c,
        {
            .layout_params{
                .dims{ruis::dim::min, ruis::dim::fill},
                .weight = 1
            }
        },
        {
            m::text(c,
                {},
                std::move(text)
            )
        }
    );
    // clang-format on
}
}

utki::shared_ref<ruis::window> make_text_input_window(
    utki::shared_ref<ruis::context> c,
    ruis::vec2_length pos
)
{
    // clang-format off
    auto new_native_window_button = m::push_button(c,
        {},
        {
            m::text(c, {}, U"new native window"s)
        }
    );
    // clang-format on

    new_native_window_button.get().click_handler = [](ruis::push_button& b){
        utki::logcat("new native window button clicked", '\n');

        auto& nw = ruisapp::inst().make_window(
            {
                .title = "new native_window"s
            }
        );

        auto c = make_new_native_window_root_widget(nw.gui.context);
        nw.gui.set_root(c);

        nw.gui.context.get().window().close_handler = [&nw](){
            utki::logcat("native window close handler called", '\n');
            ruisapp::inst().destroy_window(nw);
        };
    };

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
                        .nine_patch = c.get().loader().load<ruis::res::nine_patch>("ruis_npt_frame"s)
                    }
                },
                {
                    m::collapse_area(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::max, ruis::dim::min}
                            },
                            .title = U"Hi!"s
                        },
                        {
                            m::text(c, {}, U"Hello!"s),
                            m::row(c,
                                {},
                                {
                                    m::push_button(c,
                                        {},
                                        {
                                            m::text(c, {}, U"button!!!"s)
                                        }
                                    ),
                                    m::image_push_button(c,
                                        {
                                            .layout_params{
                                                .dims{50_pp, 50_pp}
                                            },
                                            .widget_params{
                                                .id = "image_push_button"s
                                            },
                                            .image_button_params{
                                                .unpressed_image = c.get().loader().load<ruis::res::image>("img_button_unpressed"sv),
                                                .pressed_image = c.get().loader().load<ruis::res::image>("img_button_pressed"sv)
                                            }
                                        }
                                    ),
                                    std::move(new_native_window_button)
                                }
                            )
                        }
                    )
                }
            ),
            m::gap(c,
                {
                    .layout_params{
                        .dims{0_pp, 5_pp}
                    }
                }
            ),
            m::nine_patch(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::min}
                    },
                    .widget_params{
                        .id = "text_input"s
                    },
                    .nine_patch_params{
                        .nine_patch = c.get().loader().load<ruis::res::nine_patch>("ruis_npt_textfield_background"sv)
                    }
                },
                {
                    m::text_input_line(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::max}
                            },
                            .color_params{
                                .color = c.get().style().get_color_text_normal()
                            }
                        },
                        U"Hello Wrodl!!!"s
                    )
                }
            ),
            make_push_button(c, U"button!!!"s),
            make_push_button(c, U"button!!!"s),
            make_push_button(c, U"button!!!"s),
            make_push_button(c, U"button!!!"s),
            make_push_button(c, U"button!!!"s),
            make_push_button(c, U"button!!!"s),
            make_push_button(c, U"button!!!"s),
            make_push_button(c, U"button!!!"s),
            make_push_button(c, U"button!!!"s),
            make_push_button(c, U"button!!!"s)
        }
    );
    // clang-format on
}
