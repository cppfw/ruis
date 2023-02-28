#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <mordavokne/application.hpp>

#include "../../../src/morda/widgets/button/push_button.hpp"
#include "../../../src/morda/widgets/label/busy.hpp"
#include "../../../src/morda/util/weak_widget_set.hpp"

class application : public mordavokne::application{
	static mordavokne::window_params GetWindowParams()noexcept{
		mordavokne::window_params wp(r4::vector2<unsigned>(1024, 800));

		return wp;
	}
public:

	application() :
			mordavokne::application("morda-tests", GetWindowParams())
	{
		this->gui.initStandardWidgets(*this->get_res_file("../../res/morda_res/"));

		auto c = this->gui.context.get().inflater.inflate_as<morda::container>(
				*this->get_res_file("res/test.gui")
			);
		this->gui.set_root(c);

		auto spinner = utki::make_weak(
				utki::make_shared_from(
						c.get().get_widget_as<morda::busy>("busy_spinner")
					)
			);
		auto& button = c.get().get_widget_as<morda::push_button>("busy_toggle_button");

		morda::weak_widget_set enable_widgets;

		auto pbs = c.get().get_widget_as<morda::container>("enable_group").get_all_widgets<morda::push_button>();
		LOG([&](auto&o){o << "pbs.size() = " << pbs.size() << std::endl;})
		for(auto& w : pbs){
			LOG([](auto&o){o << "adding..." << std::endl;})
			enable_widgets.add(w.to_shared_ptr());
		}

		button.click_handler = [
				spinner,
				enable_widgets{std::move(enable_widgets)}
			](morda::push_button& b) mutable
		{
			if(auto s = spinner.lock()){
				s->set_active(!s->is_visible());
				enable_widgets.set_enabled(!s->is_visible());
			}
		};
	}
};

mordavokne::application_factory app_fac([](auto args){
	return std::make_unique<::application>();
});
