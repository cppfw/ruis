#include "vertical_list_window.hpp"

#include <ruis/widget/group/list.hpp>
#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::window> make_vertical_list_window(
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
                .layout = ruis::layout::row
            },
            .title = U"VerticalList"s
        },
        {
            m::mouse_proxy(c,
                {
                    .layout_params{
                        .dims{50_pp, ruis::dim::fill}
                    },
                    .widget_params{
                        .id = "list_mouseproxy"s
                    }
                }
            ),
            m::pile(c,
                {
                    .layout_params{
                        .dims{ruis::dim::fill, ruis::dim::max},
                        .weight = 1
                    }
                },
                {
                    m::list(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::max, ruis::dim::max}
                            },
                            .widget_params{
                                .id = "list"s
                            },
                            .list_providable_params{
                                .provider = [&c](){
                                    class the_provider : public ruis::list_provider{
                                        std::vector<std::u32string> items;
                                    public:
                                        the_provider(utki::shared_ref<ruis::context> context) :
                                            list_provider(std::move(context)),
                                            items{
                                                U"item0"s,
                                                U"item_1_iequwgfiew"s,
                                                U"item_2_werf"s,
                                                U"item3"s,
                                                U"item_4"s,
                                                U"item_5"s,
                                                U"item_6"s,
                                                U"item_7"s,
                                                U"item_8"s,
                                                U"item_9_qrgfwergerwggergergergergergergergergqergwergwergerwgewrg"s,
                                                U"item_10"s,
                                                U"item_4"s,
                                                U"item_5"s,
                                                U"item_6"s,
                                                U"item_7"s,
                                                U"item_8"s,
                                                U"item_4"s,
                                                U"item_5"s,
                                                U"item_6"s,
                                                U"item_7"s,
                                                U"item_8"s,
                                                U"item_4"s,
                                                U"item_5"s,
                                                U"item_6"s,
                                                U"item_7"s,
                                                U"item_8"s,
                                                U"item_4"s,
                                                U"item_5"s,
                                                U"item_6"s,
                                                U"item_7"s,
                                                U"item_8"s,
                                            }
                                        {}

                                        size_t count()const noexcept override{
                                            return this->items.size();
                                        }

                                        utki::shared_ref<ruis::widget> get_widget(size_t index) override{
                                            if(index == 3){
                                                return m::push_button(this->context,
                                                    {},
                                                    {
                                                        m::text(this->context,
                                                            {
                                                                .color_params{
                                                                    .color = 0xff0000ff
                                                                }
                                                            },
                                                            this->items.at(index)
                                                        )
                                                    }
                                                );
                                            }else{
                                                return m::text(this->context, {}, this->items.at(index));
                                            }
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
                        .dims{ruis::dim::min, ruis::dim::max}
                    },
                    .widget_params{
                        .id = "vertical_list_slider"s
                    },
                    .oriented_params{
                        .vertical = true
                    }
                }
            )
        }
    );
    // clang-format on
}
