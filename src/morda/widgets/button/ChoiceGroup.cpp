#include "ChoiceGroup.hpp"


using namespace morda;



ChoiceGroup::ChoiceGroup(const stob::Node* chain) :
		Widget(chain),
		Pile(chain)
{
}

void ChoiceGroup::setActiveChoiceButton(const std::shared_ptr<ChoiceButton>& cb) {
	if(auto a = this->activeChoiceButton_v.lock()){
		if(a == cb){
			//given ChoiceButton is alreday active
			return;
		}
		a->setChecked(false);
	}
	this->activeChoiceButton_v = cb;
}

