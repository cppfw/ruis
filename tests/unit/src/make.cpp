#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../harness/util/dummy_context.hpp"

#include <ruis/gui.hpp>
#include <ruis/make.hpp>
#include <ruis/layouts/pile_layout.hpp>

namespace{
const tst::set set("make", [](tst::suite& suite){
    suite.add("make_empty_container", [](){
        ruis::gui gui(make_dummy_context());

        auto c = ruis::make::container(gui.context, {}, {}, {});
    });

    suite.add("basic_make", [](){
        ruis::gui gui(make_dummy_context());

        auto c = ruis::make::container(
            gui.context,
            {
                .lp = {
                    .dims = {ruis::lp::min, ruis::lp::fill}
                }
            },{
                .layout = ruis::pile_layout::instance
            },
            {
                ruis::make::widget(
                    gui.context,
                    {
                    .rectangle = {10, 20, 30, 40}
                    }
                ),
                ruis::make::widget(
                    gui.context,
                    {}
                ),
            }
        );
    });
});
}