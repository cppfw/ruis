#include "window1.hpp"

#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/group/list.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>

using namespace std::string_literals;

using namespace ruis::length_literals;

namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::window> make_window1(
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
                .layout = ruis::layout::column
            },
            .title = U"HorizontalLayout"s
        },
        {
            m::row(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::min}
                    },
                    .widget_params{
                        .id = "someContainer"s,
                        .clip = true
                    }
                },
                {
                    m::text(c,
                        {
                            .layout_params{
                                .weight = 0
                            },
                            .widget_params{
                                .id = "label1"s
                            }
                        },
                        U"trololo"s
                    ),
                    m::text(c,
                        {
                            .layout_params{
                                .weight = 1
                            },
                            .widget_params{
                                .id = "label2"s
                            }
                        },
                        U"blah bla"s
                    ),
                    m::push_button(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::min},
                                .weight = 1
                            },
                            .widget_params{
                                .id = "show_VK_button"s,
                                .rectangle{
                                    {
                                        (50_pp).get(c.get()),
                                        (80_pp).get(c.get())
                                    },
                                    {
                                        (10_pp).get(c.get()),
                                        (30_pp).get(c.get())
                                    }
                                },
                                .clip = true
                            }
                        },
                        {
                            m::text(c,
                                {
                                    .color_params{
                                        .color = 0xffccfbff
                                    }
                                },
                                U"Show VK"s
                            )
                        }
                    )
                }
            ),
            m::pile(c,
                {
                    .layout_params{
                        .dims{ruis::dim::fill, 50_pp}
                    }
                },
                {
                    m::rectangle(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::fill}
                            },
                            .color_params{
                                .color = 0xff808080
                            }
                        }
                    ),
                    m::mouse_proxy(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::fill}
                            },
                            .widget_params{
                                .id = "horizontal_list_mouseproxy"s
                            }
                        }
                    )
                }
            ),
            m::pile(c,
                {
                    .layout_params{
                        .dims{ruis::dim::fill, ruis::dim::fill},
                        .weight = 1
                    }
                },
                {
                    m::list(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::fill}
                            },
                            .widget_params{
                                .id = "pan_list"s
                            },
                            .oriented_params{
                                .vertical = false
                            },
                            .providable_params{
                                .provider = [&c](){
                                    class the_provider : public ruis::list_provider{
                                        std::vector<std::u32string> items;
                                    public:
                                        the_provider(utki::shared_ref<ruis::context> context) :
                                            list_provider(std::move(context)),
                                            items{
                                                U"item1|"s,
                                                U"item2|"s,
                                                U"item3|"s,
                                                U"item4|"s,
                                                U"item5|"s,
                                                U"item6|"s,
                                                U"item7|"s,
                                                U"item8|"s,
                                                U"item9|"s,
                                                U"item11|"s,
                                                U"item12|"s,
                                                U"item13|"s,
                                            }
                                        {}

                                        size_t count() const noexcept override{
                                            return this->items.size();
                                        }

                                        utki::shared_ref<ruis::widget> get_widget(size_t index) override{
                                            return m::text(this->context,
                                                {
                                                    .layout_params{
                                                        .dims{
                                                            ruis::dim::min,
                                                            index == 0 ? ruis::dim::max : ruis::dim::min
                                                        }
                                                    }
                                                },
                                                this->items.at(index)
                                            );
                                        }
                                    };
                                    return std::make_shared<the_provider>(c);
                                }()
                            }
                        }
                    )
                }
            ),
            m::scroll_bar(c,
                {
                    .layout_params{
                        .dims{ruis::dim::fill, ruis::dim::min}
                    },
                    .widget_params{
                        .id = "horizontal_list_slider"s
                    },
                    .oriented_params{
                        .vertical = false
                    }
                }
            )
        }
    );
    // clang-format on
}
