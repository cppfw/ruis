#include "dummy_context.hpp"

#include <ruis/render/renderer.hpp>
#include <ruis/render/null/context.hpp>

utki::shared_ref<ruis::context> make_dummy_context(){
    auto rendering_context = utki::make_shared<ruis::render::null::context>();
    auto common_objects = utki::make_shared<ruis::render::renderer::objects>(rendering_context);
    return utki::make_shared<ruis::context>(
        utki::make_shared<ruis::style_provider>(
            utki::make_shared<ruis::resource_loader>(
                rendering_context,//
                common_objects
            )
        ),
        utki::make_shared<ruis::render::renderer>(
            rendering_context,//
            rendering_context.get().make_shaders(),
            common_objects
        ),
        utki::make_shared<ruis::updater>(),
        ruis::context::parameters{
            .post_to_ui_thread_function = [](std::function<void()>){},
            .units = ruis::units(
                ruis::real(96), //
                ruis::real(1)
            )
        }
    );
}
