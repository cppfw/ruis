#include "radio_button.hpp"
#include "choice_group.hpp"

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

radio_button::radio_button(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		toggle_button(this->context, desc),
		choice_button(this->context, desc),
		Pile(this->context, D_Layout)
{
	this->checkWidget = *this->children().rbegin();
	ASSERT(this->checkWidget)
	this->checkWidget->set_visible(this->is_pressed());
}

void radio_button::on_pressed_changed(){
	this->choice_button::on_pressed_changed();
	this->checkWidget->set_visible(this->is_pressed());
	this->clear_cache();
}
