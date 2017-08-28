#include "ChoiceButton.hpp"

#include "ChoiceGroup.hpp"

using namespace morda;



ChoiceButton::ChoiceButton(const stob::Node* chain) :
		Widget(chain),
		ToggleButton(chain)
{
}


void ChoiceButton::onCheckedChanged() {
	auto cg = this->findAncestor<ChoiceGroup>();
	if(!cg){
		this->ToggleButton::onCheckedChanged();
		return;
	}
		
	if(this->isChecked()){
		if(!cg->isButtonActive(*this)){
			cg->setActiveChoiceButton(this->sharedFromThis(this));
			this->ToggleButton::onCheckedChanged();
		}
	}else{
		if(cg->isButtonActive(*this)){
			this->setChecked(true);
		}else{
			this->ToggleButton::onCheckedChanged();
		}
	}
}
