#include "tree_view_window.hpp"

#include <ruis/widget/group/tree_view.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/button/push_button.hpp>

using namespace std::string_literals;

using namespace ruis::length_literals;

namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::window> make_tree_view_window(
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
            .title = U"TreeView"s
        },
        {
            m::row(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::fill},
                        .weight = 1
                    }
                },
                {
                    m::pile(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::max},
                                .weight = 1
                            }
                        },
                        {
                            m::tree_view(c,
                                {
                                    .layout_params{
                                        .dims{ruis::dim::fill, ruis::dim::fill}
                                    },
                                    .widget_params{
                                        .id = "treeview_widget"s,
                                        .clip = true
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
                                .id = "treeview_vertical_slider"s
                            },
                            .oriented_params{
                                .vertical = true
                            }
                        }
                    )
                }
            ),
            m::scroll_bar(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::min}
                    },
                    .widget_params{
                        .id = "treeview_horizontal_slider"s
                    },
                    .oriented_params{
                        .vertical = false
                    }
                }
            ),
            m::row(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::min}
                    }
                },
                {
                    m::text(c, {}, U"Insert:"s),
                    m::push_button(c,
                        {
                            .widget_params{
                                .id = "insert_before"s
                            }
                        },
                        {
                            m::text(c, {}, U"before"s)
                        }
                    ),
                    m::push_button(c,
                        {
                            .widget_params{
                                .id = "insert_after"s
                            }
                        },
                        {
                            m::text(c, {}, U"after"s)
                        }
                    ),
                    m::push_button(c,
                        {
                            .widget_params{
                                .id = "insert_child"s
                            }
                        },
                        {
                            m::text(c, {}, U"child"s)
                        }
                    )
                }
            )
        }
    );
    // clang-format on
}
