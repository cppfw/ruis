#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <ruisapp/application.hpp>

#include <ruis/widgets/button/push_button.hpp>
#include <ruis/widgets/label/busy.hpp>

class application : public mordavokne::application{
public:
	application() :
			mordavokne::application("morda-tests", [](){
				mordavokne::window_params wp(r4::vector2<unsigned>(1024, 800));

				return wp;
			}())
	{
		this->gui.init_standard_widgets(*this->get_res_file("../../res/morda_res/"));

		auto c = this->gui.context.get().inflater.inflate(
				*this->get_res_file("res/test.gui")
			);
		this->gui.set_root(c);
	}
};

const mordavokne::application_factory app_fac([](auto args){
	return std::make_unique<::application>();
});
