#include "ChoiceButton.hpp"

#include "ChoiceGroup.hpp"

using namespace morda;



ChoiceButton::ChoiceButton(const stob::Node* chain) :
		Widget(chain)
{
}

void ChoiceButton::onCheckedChanged() {
	if(this->checkedChanged){
		this->checkedChanged(*this);
	}
}

void ChoiceButton::setChecked() {
	this->setChecked(true);
}

void ChoiceButton::setChecked(bool checked) {
	if(this->isChecked() == checked){
		return;
	}
	this->isChecked_v = checked;
	this->onCheckedChanged();
}



void ChoiceButton::onPressedChanged() {
	if(!this->isPressed()){
		return;
	}
	if(this->isChecked()){
		return;
	}
	if(auto cg = this->findAncestor<ChoiceGroup>()){
		cg->setActiveChoiceButton(this->sharedFromThis(this));
	}
	this->setChecked();
}
