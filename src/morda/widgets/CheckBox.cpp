#include "CheckBox.hpp"
#include "../App.hpp"


using namespace morda;



namespace{

const char* D_Layout = R"qwertyuiop(
		ImageLabel{
			name{morda_checkbox_check}
			image{morda_img_checkbox}
			layout{
				fillX{true} fillY{true}
			}
		}
	)qwertyuiop";

}



CheckBox::CheckBox(const stob::Node* chain) :
		Widget(chain),
		ToggleButton(chain),
		NinePatch(stob::parse(D_Layout).get())
{
	this->checkWidget = this->Content().findChildByName("morda_checkbox_check");
	this->checkWidget->setVisible(this->isChecked());
	
	this->SetNinePatch(morda::App::inst().resMan.Load<ResNinePatch>("morda_npt_checkbox"));
}



void CheckBox::onCheckedChanged(){
	this->checkWidget->setVisible(this->isChecked());
	this->clearCache();
	this->ToggleButton::onCheckedChanged();
}
