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
		virtual public button,
		public NinePatch
{
	std::shared_ptr<const res_ninepatch> unpressedNinePatch_v;
	std::shared_ptr<const res_ninepatch> pressedNinePatch_v;
	
public:
	NinePatchButton(const NinePatchButton&) = delete;
	NinePatchButton& operator=(const NinePatchButton&) = delete;
	
	void setUnpressedNinePatch(std::shared_ptr<const res_ninepatch> np);
	
	const decltype(unpressedNinePatch_v)& unpressedNinePatch()const noexcept{
		return this->unpressedNinePatch_v;
	}
	
	void setPressedNinePatch(std::shared_ptr<const res_ninepatch> np);
	
	const decltype(pressedNinePatch_v)& pressedNinePatch()const noexcept{
		return this->pressedNinePatch_v;
	}
	
protected:
	NinePatchButton(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	void on_pressed_changed()override;
};

}
