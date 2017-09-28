#include "ToggleButton.hpp"

#include "../../util/util.hpp"

using namespace morda;


ToggleButton::ToggleButton(const stob::Node* chain) :
		Widget(chain),
		Button(chain)
{
	if(const stob::Node* n = getProperty(chain, "checked")){
		this->isChecked_v = n->asBool();
	}else{
		this->isChecked_v = false;
	}
}

void ToggleButton::onCheckedChanged() {
	if (this->checkedChanged) {
		this->checkedChanged(*this);
	}
}

void ToggleButton::setChecked(bool checked) {
	if (this->isChecked_v == checked) {
		return;
	}

	this->isChecked_v = checked;
	this->onCheckedChanged();
}

void ToggleButton::onPressedChanged() {
	if(this->isPressed()){
		this->toggle();
	}
}

