#include "ChoiceGroup.hpp"


using namespace morda;



ChoiceGroup::ChoiceGroup(const stob::Node* chain) :
		Widget(chain),
		Pile(chain)
{
}

bool ChoiceGroup::isButtonActive(const ChoiceButton& b) const noexcept{
	return this->activeChoiceButton_v.get() == &b;
}


void ChoiceGroup::setActiveChoiceButton(const std::shared_ptr<ChoiceButton>& cb) {
	if(this->activeChoiceButton_v){
		this->activeChoiceButton_v->setChecked(false);
	}
	this->activeChoiceButton_v = cb;
}

