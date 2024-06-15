#include "dummy_context.hpp"

#include <ruis/render/null/renderer.hpp>

utki::shared_ref<ruis::context> make_dummy_context(){
    return utki::make_shared<ruis::context>(
            utki::make_shared<ruis::render::null::renderer>(),
            utki::make_shared<ruis::updater>(),
            [](std::function<void()>){},
            [](ruis::mouse_cursor){},
            ruis::real(96),
            ruis::real(1)
        );
}
