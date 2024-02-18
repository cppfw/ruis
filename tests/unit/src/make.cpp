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

        auto c = ruis::make::container(gui.context, {}, {}, {});
    });

    suite.add("basic_make", [](){
        ruis::gui gui(make_dummy_context());

        auto c = ruis::make::container(
            gui.context,
#if CFG_CPP >= 20
            {
                .lp = {
                    .dims = {ruis::lp::min, ruis::lp::fill}
                }
            }
#else
            ruis::widget::parameters{
                "",
                ruis::lp{
                    ruis::vector2{ruis::lp::min, ruis::lp::fill}
                }
            }
#endif
            ,
#if CFG_CPP >= 20
            {
                .layout = ruis::pile_layout::instance
            }
#else
            ruis::container::parameters{
                ruis::pile_layout::instance
            }
#endif
            ,
            {
                ruis::make::widget(
                    gui.context,
#if CFG_CPP >= 20
                    {
                        .rectangle = {10, 20, 30, 40}
                    }
#else
                    ruis::widget::parameters{
                        "",
                        {},
                        ruis::rect{10, 20, 30, 40}
                    }
#endif
                ),
                ruis::make::widget(
                    gui.context,
                    {}
                )
                // ,
                // ruis::make::text(
                //     gui.context,
                //     {},
                //     U"hello world"
                // )
                ,
                ruis::make::rectangle(
                    gui.context,
                    {},
                    {0xff0000ff}
                )
            }
        );
    }); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
});
}
