#include <utki/debug.hpp>

#include <ruisapp/application.hpp>

class application : public ruisapp::application{
public:
	application() :
			ruisapp::application("ruis-tests", [](){
				ruisapp::window_params wp(r4::vector2<unsigned>(1024, 800));

				return wp;
			}())
	{
		this->gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		// auto c = this->gui.context.get().inflater.inflate(
		// 		*this->get_res_file("res/test.gui")
		// 	);
		// this->gui.set_root(c);

		// {
		// 	auto spinner = utki::make_weak_from(
		// 			c.get().get_widget_as<ruis::busy>("busy_spinner")
		// 		);
		// 	auto& button = c.get().get_widget_as<ruis::push_button>("busy_toggle_button");

		// 	auto disable_button = utki::make_weak_from(c.get().get_widget("refresh_disable_button"));

		// 	button.click_handler = [spinner, disable_button](ruis::push_button& b){
		// 		if(auto s = spinner.lock()){
		// 			s->set_active(!s->is_visible());
		// 		}
		// 		if(auto db = disable_button.lock()){
		// 			db->set_enabled(!db->is_enabled());
		// 		}
		// 	};
		// }

		// c.get().get_widget_as<ruis::busy>("busy_spinner2").set_active(true);

		// {
		// 	auto spinner = utki::make_weak_from(c.get().get_widget_as<ruis::spinner>("refresh_spinner"));
		// 	auto& button = c.get().get_widget_as<ruis::push_button>("refresh_toggle_button");
		// 	button.click_handler = [spinner](ruis::push_button& b){
		// 		if(auto s = spinner.lock()){
		// 			s->set_active(!s->is_updating());
		// 		}
		// 	};
		// }
	}
};

const ruisapp::application_factory app_fac([](auto args){
	return std::make_unique<::application>();
});
