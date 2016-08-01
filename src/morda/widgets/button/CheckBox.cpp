#include "CheckBox.hpp"

#include "../../Morda.hpp"


using namespace morda;



namespace{

const char* D_Layout = R"qwertyuiop(
		ImageLabel{
			name{morda_checkbox_check}
			image{morda_img_checkbox_tick}
		}
	)qwertyuiop";

}



CheckBox::CheckBox(const stob::Node* chain) :
		Widget(chain),
		ToggleButton(chain),
		NinePatch(stob::parse(D_Layout).get())
{
	this->checkWidget = this->content().findChildByName("morda_checkbox_check");
	this->checkWidget->setVisible(this->isChecked());
	
	this->setNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>("morda_npt_checkbox_bg"));
}



void CheckBox::onCheckedChanged(){
	this->checkWidget->setVisible(this->isChecked());
	this->clearCache();
	this->ToggleButton::onCheckedChanged();
}
