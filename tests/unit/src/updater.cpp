#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/updater.hpp>

namespace{
const tst::set set("updater", [](tst::suite& suite){
    suite.add("stop_from_callback", [](){
        uint32_t cur_ticks = 0;

        auto updater = utki::make_shared<ruis::updater>(
            [&](){
                return cur_ticks;
            }
        );

        struct test_updateable : public ruis::updateable{
            utki::shared_ref<ruis::updater> updater;

            unsigned num_updated = 0;

            test_updateable(utki::shared_ref<ruis::updater> updater) :
                updater(std::move(updater))
            {}

            void update(uint32_t dt)override{
                ++this->num_updated;
                this->updater.get().stop(*this);
            }
        };

        auto u = utki::make_shared<test_updateable>(updater);

        tst::check(!u.get().is_updating(), SL);
        tst::check_eq(u.get().num_updated, unsigned(0), SL);

        updater.get().start(u.to_shared_ptr(), 10);

        tst::check(u.get().is_updating(), SL);

        updater.get().update();

        tst::check_eq(u.get().num_updated, unsigned(0), SL);
        tst::check(u.get().is_updating(), SL);

        cur_ticks = 11;
        updater.get().update();

        tst::check_eq(u.get().num_updated, unsigned(1), SL);
        tst::check(!u.get().is_updating(), SL);

        cur_ticks = 22;
        updater.get().update();

        tst::check_eq(u.get().num_updated, unsigned(1), SL);
        tst::check(!u.get().is_updating(), SL);
    });

    suite.add("restart_from_callback", [](){
        uint32_t cur_ticks = 0;

        auto updater = utki::make_shared<ruis::updater>(
            [&](){
                return cur_ticks;
            }
        );

        struct test_updateable : public ruis::updateable{
            utki::shared_ref<ruis::updater> updater;

            unsigned num_updated = 0;

            test_updateable(utki::shared_ref<ruis::updater> updater) :
                updater(std::move(updater))
            {}

            void update(uint32_t dt)override{
                ++this->num_updated;
                this->updater.get().stop(*this);
                this->updater.get().start(utki::make_weak_from(*this), 7);
            }
        };

        auto u = utki::make_shared<test_updateable>(updater);

        tst::check(!u.get().is_updating(), SL);
        tst::check_eq(u.get().num_updated, unsigned(0), SL);

        updater.get().start(u.to_shared_ptr(), 10);

        tst::check(u.get().is_updating(), SL);

        updater.get().update();

        tst::check_eq(u.get().num_updated, unsigned(0), SL);
        tst::check(u.get().is_updating(), SL);

        cur_ticks = 11;
        updater.get().update();

        tst::check_eq(u.get().num_updated, unsigned(1), SL);
        tst::check(u.get().is_updating(), SL);

        cur_ticks = 17;
        updater.get().update();

        tst::check_eq(u.get().num_updated, unsigned(1), SL);
        tst::check(u.get().is_updating(), SL);

        cur_ticks = 19;
        updater.get().update();

        tst::check_eq(u.get().num_updated, unsigned(2), SL);
        tst::check(u.get().is_updating(), SL);

        cur_ticks = 27;
        updater.get().update();

        tst::check_eq(u.get().num_updated, unsigned(3), SL);
        tst::check(u.get().is_updating(), SL);

        updater.get().stop(u.get());

        tst::check_eq(u.get().num_updated, unsigned(3), SL);
        tst::check(!u.get().is_updating(), SL);

        cur_ticks = 50;
        updater.get().update();

        tst::check_eq(u.get().num_updated, unsigned(3), SL);
        tst::check(!u.get().is_updating(), SL);
    });
});
}
