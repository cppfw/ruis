#include "window0.hpp"

#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/button/impl/check_box.hpp>
#include <ruis/widget/button/impl/drop_down_box.hpp>
#include <ruis/widget/button/choice_group.hpp>
#include <ruis/widget/button/impl/radio_button.hpp>

using namespace std::string_literals;

using namespace ruis::length_literals;

namespace{
namespace m{
using namespace ruis::make;
}
}

namespace{
utki::shared_ref<ruis::widget> make_radio_button(
    utki::shared_ref<ruis::context> c, //
    std::u32string text
)
{
    // clang-format off
    return m::row(c,
        {},
        {
            m::padding(c,
                {
                    .padding_params{
                        .borders{5_pp, 5_pp, 5_pp, 5_pp}
                    }
                },
                {
                    m::radio_button(c, {})
                }
            ),
            m::text(c, {}, std::move(text))
        }
    );
    // clang-format on
}
}

utki::shared_ref<ruis::window> make_window0(
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
                .cache = true
            },
            .title = U"cached"s
        },
        {
            m::column(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::max}
                    }
                },
                {
                    m::row(c,
                        {
                            .layout_params{
                                .align{ruis::align::back, ruis::align::undefined}
                            }
                        },
                        {
                            m::padding(c,
                                {
                                    .container_params{
                                        .layout = ruis::layout::pile
                                    },
                                    .padding_params{
                                        .borders{
                                            ruis::length::make_pp(5),
                                            ruis::length::make_pp(5),
                                            ruis::length::make_pp(5),
                                            ruis::length::make_pp(5)
                                        }
                                    }
                                },
                                {
                                    m::check_box(c,
                                        {
                                            .button_params{
                                                .pressed = true
                                            }
                                        }
                                    )
                                }
                            ),
                            m::text(c,
                                {
                                    .text_params{
                                        .font_size = ruis::length::make_pp(20),
                                        .font_face = c.get().loader().load<ruis::res::font>("fnt_monospace")
                                    }
                                },
                                U"some checkbox"s
                            )
                        }
                    ),
                    m::drop_down_box(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::max, ruis::dim::min}
                            },
                            .widget_params{
                                .id = "dropdownselector"s
                            },
                            .list_params{
                                .provider = [&](){
                                    class the_provider : public ruis::list_provider
                                    {
                                        std::vector<std::u32string> items;
                                    public:
                                        the_provider(utki::shared_ref<ruis::context> context) :
                                            list_provider(std::move(context)),
                                            items{
                                                U"item0"s,
                                                U"item1"s,
                                                U"item2"s,
                                                U"item3"s,
                                                U"item4"s,
                                                U"item5"s,
                                                U"item6"s,
                                                U"item7"s,
                                                U"item8"s,
                                                U"item9"s,
                                                U"item10"s
                                            }
                                        {}

                                        size_t count()const noexcept override{
                                            return this->items.size();
                                        }

                                        utki::shared_ref<ruis::widget> get_widget(size_t index)override{
                                            return m::text(this->context, {}, this->items.at(index));
                                        }
                                    };
                                    return utki::make_shared<the_provider>(c);
                                }()
                            }
                        }
                    ),
                    m::text(c,
                        {
                            .widget_params{
                                .id = "dropdownselector_selection"s
                            }
                        },
                        U"unkonwn"s
                    ),
                    m::row(c,
                        {
                            .layout_params{
                                .align{ruis::align::front, ruis::align::front}
                            }
                        },
                        {
                            m::push_button(c,
                                {
                                    .widget_params{
                                        .id = "showhide_mousecursor_button"s
                                    }
                                },
                                {
                                    m::text(c, {}, U"show/hide mouse"s)
                                }
                            ),
                            m::push_button(c,
                                {
                                    .widget_params{
                                        .id = "fullscreen_button"s
                                    }
                                },
                                {
                                    m::text(c, {}, U"toggle fullscreen"s)
                                }
                            )
                        }
                    ),
                    m::choice_group(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::max, ruis::dim::min}
                            }
                        },
                        {
                            make_radio_button(c, U"radio button 1"s),
                            make_radio_button(c, U"radio button 2"s),
                            make_radio_button(c, U"radio button 3"s)
                        }
                    )
                }
            )
        }
    );
    // clang-format on
}
