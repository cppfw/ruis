#include "RadioButton.hpp"
#include "ChoiceGroup.hpp"

#include "../../context.hpp"

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



RadioButton::RadioButton(const std::shared_ptr<morda::context>& c, const puu::forest& desc) :
		widget(c, desc),
		Button(c, desc),
		ToggleButton(c, desc),
		ChoiceButton(c, desc),
		Pile(c, D_Layout)
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