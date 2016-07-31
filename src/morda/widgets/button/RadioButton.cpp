#include "RadioButton.hpp"
#include "ChoiceGroup.hpp"

#include "../../Morda.hpp"

using namespace morda;



namespace{

const char* D_Layout = R"qwertyuiop(
		ImageLabel{
			image{morda_img_radiobutton_bg}
		}
		ImageLabel{
			image{morda_img_radiobutton_tick}
		}
	)qwertyuiop";

}



RadioButton::RadioButton(const stob::Node* chain) :
		Widget(chain),
		Frame(stob::parse(D_Layout).get())
{
	this->checkWidget = *this->children().rbegin();
	ASSERT(this->checkWidget)
	this->checkWidget->setVisible(this->isChecked());
}



void RadioButton::onCheckedChanged(){
	this->checkWidget->setVisible(this->isChecked());
	this->clearCache();
	this->ChoiceButton::onCheckedChanged();
}