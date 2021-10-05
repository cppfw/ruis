#include <tst/set.hpp>
#include <tst/check.hpp>

#include <morda/context.hpp>
#include <morda/widgets/container.hpp>
#include <morda/updater.hpp>
#include <morda/util/mouse_cursor.hpp>

#include "../../harness/fake_renderer/fake_renderer.hpp"

namespace{
tst::set set("layouting", [](tst::suite& suite){
    suite.add("layout_should_stay_valid_if_children_changed_during_layouting", []{
        class test_container : public morda::container{
        public:
            test_container(std::shared_ptr<morda::context> c) :
                    morda::widget(std::move(c), treeml::forest()),
                    morda::container(this->context, treeml::forest())
            {}

            void lay_out()override{
                // the container changes it's children during layouting
                this->clear();
                this->push_back(std::make_shared<morda::widget>(this->context, treeml::forest()));
            }
        };

        auto context = std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			);

        auto c = std::make_shared<morda::container>(context, treeml::forest());
        auto tc = std::make_shared<test_container>(context);

        tst::check(tc->is_layout_dirty(), SL);
        c->push_back(tc);
        tst::check(tc->is_layout_dirty(), SL);

        // after prforming layouting on parent container the child test_container's layout should stay valid even
        // though it changes its children during layouting
        c->lay_out();
        tst::check(!tc->is_layout_dirty(), SL);

        tc->invalidate_layout();
        tst::check(tc->is_layout_dirty(), SL);
        tst::check(c->is_layout_dirty(), SL);

        // when resizing widget to the same size it should not do anything except perform re-layouting (TODO: why should it?) in case
        // layout was dirty
        tc->resize(tc->rect().d);
        tst::check(!tc->is_layout_dirty(), SL);

        tc->invalidate_layout();
        tst::check(tc->is_layout_dirty(), SL);
        tst::check(c->is_layout_dirty(), SL);

        // when resizing widget to different size it should change it's size and call on_resize() virtual method
        // which by default does re-layouting
        tc->resize(c->rect().d + morda::vector2{1, 1});
        tst::check(!tc->is_layout_dirty(), SL);
    });
});
}
