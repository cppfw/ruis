#include "ChoiceButton.hpp"

#include "ChoiceGroup.hpp"

using namespace morda;



ChoiceButton::ChoiceButton(const stob::Node* chain) :
		Widget(chain)
{
}

void ChoiceButton::setChecked(bool checked) {
	if(checked){
		if(auto cg = this->findAncestor<ChoiceGroup>()){
			cg->setActiveChoiceButton(this->sharedFromThis(this));
		}
	}
	
	if(this->isChecked_v == checked){
		return;
	}
	
	this->isChecked_v = checked;
	this->onCheckedChanged();
}


void ChoiceButton::onCheckedChanged() {
	if (this->checkedChanged) {
		this->checkedChanged(*this, this->isChecked());
	}
}
