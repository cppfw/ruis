#pragma once

#include "../core/container/Frame.hpp"

#include "ChoiceButton.hpp"

namespace morda{


class ChoiceGroup : public Frame{
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
