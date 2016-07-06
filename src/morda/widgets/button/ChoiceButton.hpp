#pragma once

#include "Button.hpp"

namespace morda{

class ChoiceButton : public PushButton{
	friend class ChoiceGroup;
	
	bool isChecked_v = false;
	
protected:
	ChoiceButton(const stob::Node* chain = nullptr);
	
	void onClicked() override{
		this->setChecked();
	}
	
	virtual void onCheckedChanged();
public:
	ChoiceButton(const ChoiceButton&) = delete;
	ChoiceButton& operator=(const ChoiceButton&) = delete;
	
	bool isChecked()const noexcept{
		return this->isChecked_v;
	}
	
	void setChecked(){
		this->setChecked(true);
	}
	
	std::function<void(ChoiceButton&, bool)> checkedChanged;
	
private:
	void setChecked(bool checked);
};

}
