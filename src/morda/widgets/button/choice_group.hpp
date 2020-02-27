#pragma once

#include "../group/Pile.hpp"

#include "choice_button.hpp"

namespace morda{

/**
 * @brief Choice group container.
 * This is a container which is used to group several choice buttons to the same
 * choice group.
 * In the GUI script can be instantiated as "choice_group". Only available after
 * initializing standard morda widgets.
 */
class choice_group : public Pile{
	friend class choice_button;
	
	std::shared_ptr<choice_button> activeChoiceButton_v;
public:
	choice_group(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	choice_group(const choice_group&) = delete;
	choice_group& operator=(const choice_group&) = delete;
	
	bool is_active(const widget& w)const noexcept;
	
	const decltype(activeChoiceButton_v)& get_active()const noexcept{
		return this->activeChoiceButton_v;
	}
	
private:
	void setActiveChoiceButton(const std::shared_ptr<choice_button>& rb);
};

}
