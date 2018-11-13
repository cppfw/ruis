#include "CheckBox.hpp"

#include "../../Morda.hpp"


using namespace morda;



namespace{

const auto D_Layout = stob::parse(R"qwertyuiop(
		Image{
			id{morda_checkbox_check}
			image{morda_img_checkbox_tick}
		}
	)qwertyuiop");

}



CheckBox::CheckBox(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		ToggleButton(chain),
		NinePatch(D_Layout.get())
{
	this->checkWidget = this->content().findByName("morda_checkbox_check");
	this->checkWidget->setVisible(this->isPressed());

	this->setNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>("morda_npt_checkbox_bg"));
}



void CheckBox::onPressedChanged(){
	this->checkWidget->setVisible(this->isPressed());
	this->clearCache();
	this->ToggleButton::onPressedChanged();
}
