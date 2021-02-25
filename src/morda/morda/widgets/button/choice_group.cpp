#include "choice_group.hpp"

using namespace morda;

choice_group::choice_group(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		pile(this->context, desc)
{}

bool choice_group::is_active(const widget& w)const noexcept{
	widget* aw = this->activeChoiceButton_v.get();
	return aw == &w;
}

void choice_group::setActiveChoiceButton(const std::shared_ptr<choice_button>& cb){
	if(cb == this->activeChoiceButton_v){
		return;
	}
	
	auto oldactive = this->activeChoiceButton_v;
	
	this->activeChoiceButton_v = cb;
	
	if(oldactive){
		oldactive->set_pressed(false);
	}
}
