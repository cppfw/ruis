#include "RadioButton.hpp"
#include "ChoiceGroup.hpp"

#include "../../context.hpp"

using namespace morda;



namespace{

const auto D_Layout = puu::read(R"qwertyuiop(
		@image{
			image{morda_img_radiobutton_bg}
		}
		@image{
			image{morda_img_radiobutton_tick}
		}
	)qwertyuiop");

}



RadioButton::RadioButton(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		Button(this->context, desc),
		ToggleButton(this->context, desc),
		ChoiceButton(this->context, desc),
		Pile(this->context, D_Layout)
{
	this->checkWidget = *this->children().rbegin();
	ASSERT(this->checkWidget)
	this->checkWidget->set_visible(this->isPressed());
}



void RadioButton::onPressedChanged(){
	this->ChoiceButton::onPressedChanged();
	this->checkWidget->set_visible(this->isPressed());
	this->clear_cache();
}