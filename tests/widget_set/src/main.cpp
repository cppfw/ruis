#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widget/container.hpp>
#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/busy.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/util/weak_widget_set.hpp>

using namespace std::string_literals;

namespace{

namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::widget> make_root_widget(utki::shared_ref<ruis::context> c){
	// clang-format off
	return m::pile(c,
		{},
		{
			m::column(c,
				{},
				{
					m::column(c,
						{
							.widget_params{
								.id = "enable_group"s
							}
						},
						{
							m::push_button(c,
								{
									.widget_params{
										.id = "button1"s
									}
								},
								{
									m::text(c, {}, U"Button 1"s)
								}
							),
							m::push_button(c,
								{},
								{
									m::text(c, {}, U"Button 2"s)
								}
							),
							m::pile(c,
								{},
								{
									m::push_button(c,
										{},
										{
											m::text(c, {}, U"Button 3"s)
										}
									)
								}
							)
						}
					),
					m::push_button(c,
						{
							.widget_params{
								.id = "busy_toggle_button"s
							}
						},
						{
							m::text(c, {}, U"toggle busy spinner"s)
						}
					)
				}
			),
			m::busy(c,
				{
					.widget_params{
						.id = "busy_spinner"s
					}
				}
			)
		}
	);
	// clang-format on
}
}

class application : public ruisapp::application{
	ruisapp::window& window;
public:
	application() :
			ruisapp::application({
				.name = "ruis-tests"}
			),
			window(this->make_window({
					.dims = {1024, 800}
				}))
	{
		this->window.gui.context.get().window().close_handler = [this](){
			this->quit();
		};

		this->window.gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		auto c = make_root_widget(this->window.gui.context);
		this->window.gui.set_root(c);

		auto spinner = utki::make_weak(
				utki::make_shared_from(
						c.get().get_widget_as<ruis::busy>("busy_spinner")
					)
			);
		auto& button = c.get().get_widget_as<ruis::push_button>("busy_toggle_button");

		ruis::weak_widget_set enable_widgets;

		auto pbs = c.get().get_widget_as<ruis::container>("enable_group").get_all_widgets<ruis::push_button>();
		LOG([&](auto&o){o << "pbs.size() = " << pbs.size() << std::endl;})
		for(auto& w : pbs){
			LOG([](auto&o){o << "adding..." << std::endl;})
			enable_widgets.add(w.to_shared_ptr());
		}

		button.click_handler = [
				spinner,
				enable_widgets{std::move(enable_widgets)}
			](ruis::push_button& b) mutable
		{
			if(auto s = spinner.lock()){
				s->set_active(!s->is_visible());
				enable_widgets.set_enabled(!s->is_visible());
			}
		};
	}
};

const ruisapp::application_factory app_fac([](auto executbale, auto args){
	return std::make_unique<::application>();
});
