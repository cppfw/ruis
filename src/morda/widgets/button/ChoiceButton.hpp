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
class ChoiceButton : public ToggleButton{
	friend class ChoiceGroup;
	
protected:
	ChoiceButton(const stob::Node* chain = nullptr);
	
	void onCheckedChanged()override;
	void onPressedChanged() override;

public:
	ChoiceButton(const ChoiceButton&) = delete;
	ChoiceButton& operator=(const ChoiceButton&) = delete;
	
private:
};

}
