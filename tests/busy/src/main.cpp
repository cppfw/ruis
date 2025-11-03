#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <fsif/native_file.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/busy.hpp>

#include "root_gui.hpp"

class application : public ruisapp::application{
	ruisapp::window& window;
public:
	application() :
			ruisapp::application(
				{
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

		auto c = make_root_gui(this->window.gui.context);
		this->window.gui.set_root(c);

		{
			auto spinner = utki::make_weak_from(
					c.get().get_widget_as<ruis::busy>("busy_spinner")
				);
			auto& button = c.get().get_widget_as<ruis::push_button>("busy_toggle_button");

			auto disable_button = utki::make_weak_from(c.get().get_widget("refresh_disable_button"));

			button.click_handler = [spinner, disable_button](ruis::push_button& b){
				if(auto s = spinner.lock()){
					s->set_active(!s->is_visible());
				}
				if(auto db = disable_button.lock()){
					db->set_enabled(!db->is_enabled());
				}
			};
		}

		c.get().get_widget_as<ruis::busy>("busy_spinner2").set_active(true);

		{
			auto spinner = utki::make_weak_from(c.get().get_widget_as<ruis::spinner>("refresh_spinner"));
			auto& button = c.get().get_widget_as<ruis::push_button>("refresh_toggle_button");
			button.click_handler = [spinner](ruis::push_button& b){
				if(auto s = spinner.lock()){
					s->set_active(!s->is_updating());
				}
			};
		}
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args){
	return std::make_unique<::application>();
});
