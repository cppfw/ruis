#include "CheckBox.hpp"

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



CheckBox::CheckBox(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		ToggleButton(this->context, desc),
		NinePatch(this->context, D_Layout)
{
	this->checkWidget = this->content().try_get_widget("morda_checkbox_check");
	this->checkWidget->set_visible(this->is_pressed());

	this->setNinePatch(this->context->loader.load<res_ninepatch>("morda_npt_checkbox_bg"));
}



void CheckBox::on_pressed_changed(){
	this->checkWidget->set_visible(this->is_pressed());
	this->clear_cache();
	this->ToggleButton::on_pressed_changed();
}
