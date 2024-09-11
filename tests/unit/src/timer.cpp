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

        unsigned num_triggered = 0;
        uint32_t time_elapsed_ms = 0;

        auto timer = utki::make_shared<ruis::timer>(
            updater,
            [&](uint32_t elapsed_ms){
                ++num_triggered;
                time_elapsed_ms = elapsed_ms;
            }
        );

        tst::check_eq(timer.get().get_timeout_ms(), uint32_t(0), SL);

        updater.get().update();

        tst::check_eq(num_triggered, unsigned(0), SL);
        tst::check(!timer.get().is_running(), SL);
        tst::check_eq(time_elapsed_ms, unsigned(0), SL);

        cur_ticks = 20;

        updater.get().update();

        tst::check_eq(num_triggered, unsigned(0), SL);
        tst::check(!timer.get().is_running(), SL);
        tst::check_eq(time_elapsed_ms, unsigned(0), SL);

        timer.get().start(10);

        tst::check_eq(timer.get().get_timeout_ms(), uint32_t(10), SL);

        tst::check_eq(num_triggered, unsigned(0), SL);
        tst::check(timer.get().is_running(), SL);
        tst::check_eq(time_elapsed_ms, unsigned(0), SL);

        cur_ticks = 28;
        updater.get().update();

        tst::check_eq(num_triggered, unsigned(0), SL);
        tst::check(timer.get().is_running(), SL);
        tst::check_eq(time_elapsed_ms, unsigned(0), SL);

        cur_ticks = 30;
        updater.get().update();

        tst::check_eq(num_triggered, unsigned(1), SL);
        tst::check(!timer.get().is_running(), SL);

        tst::check_eq(time_elapsed_ms, uint32_t(10), SL);
    });

    suite.add("extra_ms", [](){
        uint32_t cur_ticks = 0;

        auto updater = utki::make_shared<ruis::updater>(
            [&](){
                return cur_ticks;
            }
        );

        unsigned num_triggered = 0;
        uint32_t time_elapsed_ms = 0;

        auto timer = utki::make_shared<ruis::timer>(
            updater,
            [&](uint32_t elapsed_ms){
                ++num_triggered;
                time_elapsed_ms = elapsed_ms;
            }
        );

        cur_ticks = 20;
        updater.get().update();

        timer.get().start(10);

        cur_ticks = 28;
        updater.get().update();

        tst::check_eq(num_triggered, unsigned(0), SL);
        tst::check(timer.get().is_running(), SL);
        tst::check_eq(time_elapsed_ms, unsigned(0), SL);

        cur_ticks = 37;
        updater.get().update();

        tst::check_eq(num_triggered, unsigned(1), SL);
        tst::check(!timer.get().is_running(), SL);

        tst::check_eq(time_elapsed_ms, uint32_t(17), SL);
    });

    // TODO: add test: stopping before expired

    suite.add("restart_from_callback", [](){
        uint32_t cur_ticks = 0;

        auto updater = utki::make_shared<ruis::updater>(
            [&](){
                return cur_ticks;
            }
        );

        unsigned num_triggered = 0;

        utki::shared_ref<ruis::timer> timer = utki::make_shared<ruis::timer>(
            updater,
            [&](uint32_t elapsed_ms){
                ++num_triggered;
                timer.get().start(10);
            }
        );

        tst::check_eq(num_triggered, unsigned(0), SL);

        timer.get().start(10);

        tst::check_eq(num_triggered, unsigned(0), SL);

        cur_ticks = 9;
        updater.get().update();

        tst::check_eq(num_triggered, unsigned(0), SL);

        cur_ticks = 11;
        updater.get().update();

        tst::check_eq(num_triggered, unsigned(1), SL);

        cur_ticks = 22;
        updater.get().update();

        tst::check_eq(num_triggered, unsigned(2), SL);

        cur_ticks = 33;
        updater.get().update();

        tst::check_eq(num_triggered, unsigned(3), SL);
    });

    // TODO: add test: stopping after expired

    // TODO: add test: restarting after expired

    // TODO: add test: timeout more than 65535 ms
});
}
