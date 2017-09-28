#include "RadioButton.hpp"
#include "ChoiceGroup.hpp"

#include "../../Morda.hpp"

using namespace morda;



namespace{

const char* D_Layout = R"qwertyuiop(
		Image{
			image{morda_img_radiobutton_bg}
		}
		Image{
			image{morda_img_radiobutton_tick}
		}
	)qwertyuiop";

}



RadioButton::RadioButton(const stob::Node* chain) :
		Widget(chain),
		Button(chain),
		ToggleButton(chain),
		ChoiceButton(chain),
		Pile(stob::parse(D_Layout).get())
{
	this->checkWidget = *this->children().rbegin();
	ASSERT(this->checkWidget)
	this->checkWidget->setVisible(this->isPressed());
}



void RadioButton::onPressedChanged(){
	this->ChoiceButton::onPressedChanged();
	this->checkWidget->setVisible(this->isPressed());
	this->clearCache();
}