#include "dummy_context.hpp"

#include "fake_renderer.hpp"

utki::shared_ref<morda::context> make_dummy_context(){
    return utki::make_shared_ref<morda::context>(
            utki::make_shared_ref<fake_renderer>(),
            utki::make_shared_ref<morda::updater>(),
            [](std::function<void()>&&){},
            [](morda::mouse_cursor){},
            morda::real(0),
            morda::real(0)
        );
}
