#include <tst/set.hpp>
#include <tst/check.hpp>

#include <thread>

#include <ruis/util/timer.hpp>

namespace{
const tst::set set("timer", [](tst::suite& suite){
    suite.add("basic", [](){
        auto updater = utki::make_shared<ruis::updater>();

        bool triggered = false;
        uint32_t time_elapsed_ms;

        auto timer = utki::make_shared<ruis::timer>(
            updater,
            [&](uint32_t elapsed_ms){
                triggered = true;
                time_elapsed_ms = elapsed_ms;
            }
        );

        updater.get().update();

        tst::check(!triggered, SL);

        timer.get().start(10);

        tst::check(!triggered, SL);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        updater.get().update();

        tst::check(triggered, SL);
        tst::check_ge(time_elapsed_ms, uint32_t(10), SL);
    });

    // TODO:
});
}
