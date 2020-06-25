#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <mordavokne/application.hpp>

#include "../../../src/morda/morda/widgets/button/push_button.hpp"
#include "../../../src/morda/morda/widgets/label/busy.hpp"
#include "../../../src/morda/morda/util/widget_set.hpp"

class application : public mordavokne::application{
	static mordavokne::window_params GetWindowParams()noexcept{
		mordavokne::window_params wp(r4::vec2ui(1024, 800));

		return wp;
	}
public:

	application() :
			mordavokne::application("morda-tests", GetWindowParams())
	{
		this->gui.initStandardWidgets(*this->get_res_file("../../res/morda_res/"));

		auto c = this->gui.context->inflater.inflate_as<morda::container>(
				*this->get_res_file("res/test.gui")
			);
		this->gui.set_root(c);

		auto spinner = utki::make_weak(
				utki::make_shared_from_this(
						c->get_widget_as<morda::busy>("busy_spinner")
					)
			);
		auto& button = c->get_widget_as<morda::push_button>("busy_toggle_button");

		auto enable_widgets = std::make_shared<morda::widget_set>();

		auto pbs = c->get_widget_as<morda::container>("enable_group").get_all_widgets<morda::push_button>();
		TRACE(<< "pbs.size() = " << pbs.size() << std::endl)
		for(auto& w : pbs){
			TRACE(<< "adding..." << std::endl)
			enable_widgets->add(w);
		}

		button.click_handler = [spinner, enable_widgets](morda::push_button& b){
			if(auto s = spinner.lock()){
				s->set_active(!s->is_visible());
				enable_widgets->set_enabled(!s->is_visible());
			}
		};
	}
};

std::unique_ptr<mordavokne::application> mordavokne::create_application(int argc, const char** argv){
	return std::make_unique<::application>();
}
