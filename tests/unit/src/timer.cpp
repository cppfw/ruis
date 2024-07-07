#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/util/timer.hpp>

namespace{
const tst::set set("timer", [](tst::suite& suite){
    suite.add("basic", [](){
        uint32_t cur_ticks = 0;

        auto updater = utki::make_shared<ruis::updater>(
            [&](){
                return cur_ticks;
            }
        );

        bool triggered = false;
        uint32_t time_elapsed_ms = 0;

        auto timer = utki::make_shared<ruis::timer>(
            updater,
            [&](uint32_t elapsed_ms){
                triggered = true;
                time_elapsed_ms = elapsed_ms;
            }
        );

        tst::check_eq(timer.get().get_timeout_ms(), uint32_t(0), SL);

        updater.get().update();

        tst::check(!triggered, SL);
        tst::check(!timer.get().is_running(), SL);

        cur_ticks = 20;

        updater.get().update();

        tst::check(!triggered, SL);
        tst::check(!timer.get().is_running(), SL);

        timer.get().start(10);

        tst::check_eq(timer.get().get_timeout_ms(), uint32_t(10), SL);

        tst::check(!triggered, SL);
        tst::check(timer.get().is_running(), SL);

        cur_ticks = 28;
        updater.get().update();

        tst::check(!triggered, SL);
        tst::check(timer.get().is_running(), SL);

        cur_ticks = 30;
        updater.get().update();

        tst::check(triggered, SL);
        tst::check(!timer.get().is_running(), SL);

        tst::check_ge(time_elapsed_ms, uint32_t(10), SL);
    });

    // TODO: add test: stopping before expired

    // TODO: add test: restarting from callback

    // TODO: add test: stopping after expired

    // TODO: add test: restarting after expired
});
}
