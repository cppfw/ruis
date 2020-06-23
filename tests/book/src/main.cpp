#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <mordavokne/application.hpp>

#include "../../../src/morda/morda/widgets/button/push_button.hpp"
#include "../../../src/morda/morda/widgets/label/busy.hpp"
#include "../../../src/morda/morda/widgets/group/book.hpp"

#include "main_page.hpp"

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

		this->gui.context->loader.mount_res_pack(*this->get_res_file("res/"));

		std::shared_ptr<morda::widget> c = this->gui.context->inflater.inflate(
				*this->get_res_file("res/test.gui")
			);
		this->gui.set_root(c);

		auto& b = c->get_widget_as<morda::book>("book");

		b.push(std::make_shared<main_page>(this->gui.context));
	}
};

std::unique_ptr<mordavokne::application> mordavokne::create_application(int argc, const char** argv){
	return std::make_unique<::application>();
}
