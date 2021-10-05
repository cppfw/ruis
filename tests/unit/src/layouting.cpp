#include <tst/set.hpp>
#include <tst/check.hpp>

#include <morda/context.hpp>
#include <morda/widgets/container.hpp>
#include <morda/updater.hpp>
#include <morda/util/mouse_cursor.hpp>

#include "../../harness/fake_renderer/fake_renderer.hpp"

namespace{
tst::set set("layouting", [](tst::suite& suite){
    suite.add("layout_should_stay_valid_if_children_changed_during_lay_outing", []{
        class test_container : public morda::container{
        public:
            test_container(std::shared_ptr<morda::context> c) :
                    morda::widget(std::move(c), treeml::forest()),
                    morda::container(this->context, treeml::forest())
            {}

            void lay_out()override{
                // TODO:
            }
        };

        test_container c(std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			));

        // TODO:

        tst::check(!c.is_layout_invalid(), SL);
    });
});
}
