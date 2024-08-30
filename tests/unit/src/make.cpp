#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../harness/util/dummy_context.hpp"

#include <utki/config.hpp>

#include <ruis/gui.hpp>
#include <ruis/layout/pile_layout.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/label/gap.hpp>

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
                .layout_params = {
                    .dims = {ruis::dim::min, ruis::dim::fill}
                },
                .container_params = {
                    .layout = ruis::layout::pile
                }
            },
            {
                ruis::make::gap(
                    gui.context,
                    {
                        .widget_params = {
                            .rectangle = {10, 20, 30, 40}
                        }
                    }
                ),
                ruis::make::gap(
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
