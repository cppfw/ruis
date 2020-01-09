#pragma once

#include "PushButton.hpp"
#include "../label/NinePatch.hpp"

namespace morda{

/**
 * @brief Simple push button.
 * Implementation of a push button as a nine patch which can have children.
 * In GUI scripts can be instantiated as "SimpleButton". Only available after
 * initializing standard morda widgets.
 * Since the button is a NinePatch it can have children.
 */
class NinePatchButton :
		virtual public Button,
		public NinePatch
{
	std::shared_ptr<const ResNinePatch> unpressedNinePatch_v;
	std::shared_ptr<const ResNinePatch> pressedNinePatch_v;
	
public:

	NinePatchButton(const NinePatchButton&) = delete;
	NinePatchButton& operator=(const NinePatchButton&) = delete;
	
	void setUnpressedNinePatch(std::shared_ptr<const ResNinePatch> np);
	
	const decltype(unpressedNinePatch_v)& unpressedNinePatch()const noexcept{
		return this->unpressedNinePatch_v;
	}
	
	void setPressedNinePatch(std::shared_ptr<const ResNinePatch> np);
	
	const decltype(pressedNinePatch_v)& pressedNinePatch()const noexcept{
		return this->pressedNinePatch_v;
	}
	
protected:
	NinePatchButton(const puu::forest& desc);
	NinePatchButton(const stob::Node* chain) : NinePatchButton(stob_to_puu(chain)){}
	
	void onPressedChanged()override;
};

}
