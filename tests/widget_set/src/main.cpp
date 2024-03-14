#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widgets/container.hpp>
#include <ruis/widgets/button/base/push_button.hpp>
#include <ruis/widgets/label/busy.hpp>
#include <ruis/util/weak_widget_set.hpp>

class application : public ruisapp::application{
public:

	application() :
			ruisapp::application("ruis-tests", [](){
				ruisapp::window_params wp(r4::vector2<unsigned>(1024, 800));

				return wp;
			}())
	{
		this->gui.init_standard_widgets(*this->get_res_file("../../res/ruis_res/"));

		auto c = this->gui.context.get().inflater.inflate_as<ruis::container>(
				*this->get_res_file("res/test.gui")
			);
		this->gui.set_root(c);

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

const ruisapp::application_factory app_fac([](auto args){
	return std::make_unique<::application>();
});
