#include "check_box.hpp"

#include "../../context.hpp"

using namespace morda;

namespace{

const auto D_Layout = puu::read(R"qwertyuiop(
		@image{
			id{morda_checkbox_check}
			image{morda_img_checkbox_tick}
		}
	)qwertyuiop");

}

check_box::check_box(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		toggle_button(this->context, desc),
		nine_patch(this->context, D_Layout)
{
	this->checkWidget = this->content().try_get_widget("morda_checkbox_check");
	this->checkWidget->set_visible(this->is_pressed());

	this->set_nine_patch(this->context->loader.load<res_nine_patch>("morda_npt_checkbox_bg"));
}

void check_box::on_pressed_changed(){
	this->checkWidget->set_visible(this->is_pressed());
	this->clear_cache();
	this->toggle_button::on_pressed_changed();
}
