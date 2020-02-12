#include "ChoiceGroup.hpp"


using namespace morda;



ChoiceGroup::ChoiceGroup(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		Pile(this->context, desc)
{}

bool ChoiceGroup::isWidgetActive(const Widget& w) const noexcept{
	Widget* aw = this->activeChoiceButton_v.get();
	return aw == &w;
}


void ChoiceGroup::setActiveChoiceButton(const std::shared_ptr<ChoiceButton>& cb) {
	if(cb == this->activeChoiceButton_v){
		return;
	}
	
	auto oldactive = this->activeChoiceButton_v;
	
	this->activeChoiceButton_v = cb;
	
	if(oldactive){
		oldactive->setPressed(false);
	}
}

