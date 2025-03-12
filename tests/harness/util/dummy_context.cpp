#include "dummy_context.hpp"

#include <ruis/render/renderer.hpp>
#include <ruis/render/null/context.hpp>

utki::shared_ref<ruis::context> make_dummy_context(){
    return utki::make_shared<ruis::context>(
        utki::make_shared<ruis::resource_loader>(
            utki::make_shared<ruis::render::renderer>(
                utki::make_shared<ruis::render::null::context>()
            )
        ),
        utki::make_shared<ruis::updater>(),
        ruis::context::parameters{
            .post_to_ui_thread_function = [](std::function<void()>){},
            .set_mouse_cursor_function = [](ruis::mouse_cursor){},
            .units = ruis::units(
                ruis::real(96), //
                ruis::real(1)
            )
        }
    );
}
