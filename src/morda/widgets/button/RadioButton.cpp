#include "RadioButton.hpp"
#include "ChoiceGroup.hpp"

#include "../../App.hpp"

using namespace morda;



namespace{

const char* D_Layout = R"qwertyuiop(
		ImageLabel{
			name{morda_radio_check}
			image{morda_img_checkbox_tick}
		}
	)qwertyuiop";

}



RadioButton::RadioButton(const stob::Node* chain) :
		Widget(chain),
		ChoiceButton(chain),
		NinePatch(stob::parse(D_Layout).get())
{
	this->checkWidget = this->content().findChildByName("morda_radio_check");
	ASSERT(this->checkWidget)
	this->checkWidget->setVisible(this->isChecked());
	
	this->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>("morda_npt_checkbox_bg"));
}



void RadioButton::onCheckedChanged(){
	this->checkWidget->setVisible(this->isChecked());
	this->clearCache();
	this->ChoiceButton::onCheckedChanged();
}