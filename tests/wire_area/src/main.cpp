#include <r4/quaternion.hpp>
#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

#include <mordavokne/application.hpp>

#include "wire_area.hpp"
#include "wire_socket.hpp"

class application : public mordavokne::application{
	static mordavokne::window_params GetWindowParams()noexcept{
		mordavokne::window_params wp(r4::vector2<unsigned>(640, 480));

		return wp;
	}
public:
	application() :
			mordavokne::application("morda-tests", GetWindowParams())
	{
		this->gui.initStandardWidgets(*this->get_res_file("../../res/morda_res/"));

		// this->gui.context->loader.mount_res_pack(*this->get_res_file("res/"));

		this->gui.context.get().inflater.register_widget<wire_area>("wire_area");
		this->gui.context.get().inflater.register_widget<wire_socket>("wire_socket");

		auto c = this->gui.context.get().inflater.inflate(
				*this->get_res_file("res/main.gui")
			);
		this->gui.set_root(c);
	}
};

mordavokne::application_factory app_fac([](auto args){
	return std::make_unique<::application>();
});
