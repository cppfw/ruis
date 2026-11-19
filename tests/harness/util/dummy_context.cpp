#include "dummy_context.hpp"

#include <ruis/render/null/context.hpp>
#include <ruis/render/null/factory.hpp>
#include <ruis/render/renderer.hpp>

utki::shared_ref<ruis::context> make_dummy_context()
{
	auto render_factory = utki::make_shared<ruis::render::null::factory>();
	auto rendering_context = utki::make_shared<ruis::render::null::context>(
		utki::make_shared<ruis::render::native_window>(), //
		render_factory
	);
	auto common_objects = utki::make_shared<ruis::render::renderer::objects>(render_factory.get());
	return utki::make_shared<ruis::context>(ruis::context::parameters{
		.post_to_ui_thread_function = [](std::function<void()>) {},
		.updater = utki::make_shared<ruis::updater>(),
		.renderer = utki::make_shared<ruis::render::renderer>(
			rendering_context, //
			render_factory.get().make_shaders(),
			common_objects
		),
		.style_provider = utki::make_shared<ruis::style_provider>(utki::make_shared<ruis::resource_loader>(
			render_factory, //
			common_objects
		)),
		.units = ruis::units(
			ruis::real(96), //
			ruis::real(1)
		)
	});
}
