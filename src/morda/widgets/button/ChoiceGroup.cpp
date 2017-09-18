#include "ChoiceGroup.hpp"


using namespace morda;



ChoiceGroup::ChoiceGroup(const stob::Node* chain) :
		Widget(chain),
		Pile(chain)
{
}

bool ChoiceGroup::isWidgetActive(const Widget& w) const noexcept{
	Widget* aw = this->activeChoiceButton_v.get();
	return aw == &w;
}


void ChoiceGroup::setActiveChoiceButton(const std::shared_ptr<ChoiceButton>& cb) {
	if(this->activeChoiceButton_v){
		this->activeChoiceButton_v->setChecked(false);
	}
	this->activeChoiceButton_v = cb;
}

