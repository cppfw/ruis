#include "RadioButton.hpp"
#include "ChoiceGroup.hpp"

#include "../../Morda.hpp"

using namespace morda;



namespace{

const auto D_Layout = puu::read(R"qwertyuiop(
		Image{
			image{morda_img_radiobutton_bg}
		}
		Image{
			image{morda_img_radiobutton_tick}
		}
	)qwertyuiop");

}



RadioButton::RadioButton(const puu::forest& desc) :
		widget(desc),
		Button(desc),
		ToggleButton(desc),
		ChoiceButton(desc),
		Pile(D_Layout)
{
	this->checkWidget = *this->children().rbegin();
	ASSERT(this->checkWidget)
	this->checkWidget->set_visible(this->isPressed());
}



void RadioButton::onPressedChanged(){
	this->ChoiceButton::onPressedChanged();
	this->checkWidget->set_visible(this->isPressed());
	this->clearCache();
}