#include "CheckBox.hpp"

#include "../../context.hpp"


using namespace morda;



namespace{

const auto D_Layout = puu::read(R"qwertyuiop(
		Image{
			id{morda_checkbox_check}
			image{morda_img_checkbox_tick}
		}
	)qwertyuiop");

}



CheckBox::CheckBox(const puu::forest& desc) :
		widget(desc),
		Button(desc),
		ToggleButton(desc),
		NinePatch(D_Layout)
{
	this->checkWidget = this->content().try_get_widget("morda_checkbox_check");
	this->checkWidget->set_visible(this->isPressed());

	this->setNinePatch(morda::gui::inst().context->loader.load<ResNinePatch>("morda_npt_checkbox_bg"));
}



void CheckBox::onPressedChanged(){
	this->checkWidget->set_visible(this->isPressed());
	this->clearCache();
	this->ToggleButton::onPressedChanged();
}
