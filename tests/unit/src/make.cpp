#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../harness/util/dummy_context.hpp"

#include <utki/config.hpp>

#include <ruis/gui.hpp>
#include <ruis/layouts/pile_layout.hpp>
#include <ruis/widgets/label/rectangle.hpp>

namespace{
// NOLINTNEXTLINE(cppcoreguidelines-interfaces-global-init)
const tst::set set("make", [](tst::suite& suite){
    suite.add("make_empty_container", [](){
        ruis::gui gui(make_dummy_context());

        auto c = ruis::make::container(gui.context, {}, {});
    });

    suite.add("basic_make", [](){
        ruis::gui gui(make_dummy_context());

        auto c = ruis::make::container(
            gui.context,
            {
                .widget_params = {
                    .lp = {
                        .dims = {ruis::lp::min, ruis::lp::fill}
                    }
                },
                .container_params = {
                    .layout = ruis::layout::pile
                }
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
                ruis::make::rectangle(
                    gui.context,
                    {
                        .color_params = {
                            .color = 0xff0000ff
                        }
                    }
                )
            }
        );
    }); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
});
}
