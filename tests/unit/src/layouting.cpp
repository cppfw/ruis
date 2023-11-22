#include <tst/set.hpp>
#include <tst/check.hpp>

#include <morda/context.hpp>
#include <morda/container.hpp>
#include <morda/updater.hpp>
#include <morda/util/mouse_cursor.hpp>
#include <morda/gui.hpp>

#include "../../harness/util/dummy_context.hpp"

namespace{
class container_which_invalidates_its_layout_during_layouting : public morda::container{
public:
    container_which_invalidates_its_layout_during_layouting(
                const utki::shared_ref<morda::context>& c
        ) :
            morda::widget(c, treeml::forest()),
            morda::container(this->context, treeml::forest())
    {}

    void on_lay_out()override{
        this->invalidate_layout();
    }
};
}

namespace{
const tst::set set("layouting", [](tst::suite& suite){
    suite.add("invalidate_layout_during_layouting_should_result_in_dirty_layout__lay_out_method", []{
        auto context = make_dummy_context();

        auto c = std::make_shared<morda::container>(context, treeml::forest());
        auto tc = utki::make_shared<container_which_invalidates_its_layout_during_layouting>(context);

        tst::check(tc.get().is_layout_dirty(), SL);
        c->push_back(tc);
        tst::check(tc.get().is_layout_dirty(), SL);

        // after performing layouting on parent container the child container's layout should be dirty
        // because it invalidates its layout during layouting in its on_lay_out() overridden method
        c->lay_out();
        tst::check(tc.get().is_layout_dirty(), SL);
    });

    suite.add("invalidate_layout_during_layouting_should_result_in_dirty_layout__resize_to_same_size", []{
        auto context = make_dummy_context();

        auto c = std::make_shared<morda::container>(context, treeml::forest());
        auto tc = utki::make_shared<container_which_invalidates_its_layout_during_layouting>(context);

        tst::check(tc.get().is_layout_dirty(), SL);
        c->push_back(tc);
        tst::check(tc.get().is_layout_dirty(), SL);

        // when resizing widget to the same size it should not do anything except perform re-layouting (TODO: why should it?) in case
        // layout was dirty
        tc.get().resize(tc.get().rect().d);
        tst::check(tc.get().is_layout_dirty(), SL);
    });

    suite.add("invalidate_layout_during_layouting_should_result_in_dirty_layout__resize_to_different_size", []{
        auto context = make_dummy_context();

        auto c = std::make_shared<morda::container>(context, treeml::forest());
        auto tc = utki::make_shared<container_which_invalidates_its_layout_during_layouting>(context);

        tst::check(tc.get().is_layout_dirty(), SL);
        c->push_back(tc);
        tst::check(tc.get().is_layout_dirty(), SL);

        // when resizing widget to different size it should change it's size and call on_resize() virtual method
        // which by default does re-layouting
        tc.get().resize(c->rect().d + morda::vector2{1, 1});
        tst::check(tc.get().is_layout_dirty(), SL);
    });

    suite.add("invalidate_layout_during_layouting_should_result_in_dirty_layout__gui_render", []{
        auto context = make_dummy_context();

        auto tc = utki::make_shared<container_which_invalidates_its_layout_during_layouting>(context);

        tst::check(tc.get().is_layout_dirty(), SL);
        
        // when rendering GUI it should perform layoputing in case it was dirty
        morda::gui gui(context);
        gui.set_root(tc);
        
        // after setting widget as gui root it will be resized to window size and hence it will be layed out
        tst::check(tc.get().is_layout_dirty(), SL);

        tc.get().invalidate_layout();

        // gui render method will check if layout is dirty and perform re-layouting
        gui.render();
        tst::check(tc.get().is_layout_dirty(), SL);
    });
});
}
