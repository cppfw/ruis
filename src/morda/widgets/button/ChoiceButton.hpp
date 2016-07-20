#pragma once

#include "Button.hpp"

namespace morda{

/**
 * @brief Basic class of a choice button.
 * Choice button is a button which can be checked and belongs to "choice group" of
 * similar buttons and only one button from that group can be checked.
 * When some unchecked button is checked by the user the previously checked button
 * from the same choice group will be unchecked automatically. Well known example
 * of such button is a radio button.
 */
class ChoiceButton : public PushButton{
	friend class ChoiceGroup;
	
	bool isChecked_v = false;
	
protected:
	ChoiceButton();
	
	void onClicked() override{
		this->setChecked();
	}
	
	/**
	 * @brief Invoked when checked state changes.
	 * Default implementation emits 'checkedChanged' signal.
	 */
	virtual void onCheckedChanged();
public:
	ChoiceButton(const ChoiceButton&) = delete;
	ChoiceButton& operator=(const ChoiceButton&) = delete;
	
	/**
	 * @brief Tell if button is currently checked.
	 * @return true if button is checked.
	 * @return false otherwise.
	 */
	bool isChecked()const noexcept{
		return this->isChecked_v;
	}
	
	/**
	 * @brief Set this button to be checked.
	 * May result in invokation of onCheckedChanged() if checked state will change.
	 */
	void setChecked(){
		this->setChecked(true);
	}
	
	/**
	 * @brief Checked changed signal.
	 * Emitted when checked state of this button changes.
	 */
	std::function<void(ChoiceButton&, bool)> checkedChanged;
	
private:
	void setChecked(bool checked);
};

}
