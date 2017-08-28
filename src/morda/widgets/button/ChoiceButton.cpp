#include "ChoiceButton.hpp"

#include "ChoiceGroup.hpp"

using namespace morda;



ChoiceButton::ChoiceButton(const stob::Node* chain) :
		Widget(chain),
		ToggleButton(chain)
{
}

void ChoiceButton::onPressedChanged() {
	if(this->isChecked()){
		return;
	}
	this->ToggleButton::onPressedChanged();
}


void ChoiceButton::onCheckedChanged() {
	if(this->isChecked()){
		if(auto cg = this->findAncestor<ChoiceGroup>()){
			cg->setActiveChoiceButton(this->sharedFromThis(this));
		}
	}
	
	this->ToggleButton::onCheckedChanged();
}
