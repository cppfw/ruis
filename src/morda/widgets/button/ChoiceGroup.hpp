#pragma once

#include "../core/container/Pile.hpp"

#include "ChoiceButton.hpp"

namespace morda{

/**
 * @brief Choice group container.
 * This is a container which is used to group several choice buttons to the same
 * choice group.
 * In the GUI script can be instantiated as "ChoiceGroup". Only available after
 * initializing standard morda widgets.
 */
class ChoiceGroup : public Pile{
	friend class ChoiceButton;
	
	std::weak_ptr<ChoiceButton> activeChoiceButton_v;
public:
	ChoiceGroup(const stob::Node* chain = nullptr);
	
	ChoiceGroup(const ChoiceGroup&) = delete;
	ChoiceGroup& operator=(const ChoiceGroup&) = delete;
	
private:

	void setActiveChoiceButton(const std::shared_ptr<ChoiceButton>& rb);
};

}
