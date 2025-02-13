#include "window0.hpp"

#include <ruis/widget/label/margins.hpp>
#include <ruis/widget/button/impl/check_box.hpp>
#include <ruis/widget/button/impl/drop_down_box.hpp>

using namespace std::string_literals;

namespace{
namespace m{
using namespace ruis::make;
}
}

utki::shared_ref<ruis::window> make_window0(utki::shared_ref<ruis::context> c){
    // clang-format off
    return m::window(c,
        {
            .widget_params{
                .rectangle{{0, 0}, {ruis::length::make_pp(300).get(c.get()), ruis::length::make_pp(200).get(c.get())}},
                .cache = true
            },
            .container_params{
                .layout = ruis::layout::pile
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
                            m::margins(c,
                                {
                                    .container_params{
                                        .layout = ruis::layout::pile
                                    },
                                    .frame_params{
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
                                        .font_face = c.get().loader.load<ruis::res::font>("fnt_monospace")
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
                            .providable_params{
                                .provider = [&]() -> std::shared_ptr<ruis::list_provider>{
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
                                    return std::make_shared<the_provider>(c);
                                }()
                            }
                        }
                    )
                }
            )
        }
    );
    // clang-format on
}
