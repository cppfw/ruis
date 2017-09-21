#pragma once

#include "../NinePatch.hpp"
#include "ChoiceButton.hpp"

namespace morda{
class Tab :
		public ChoiceButton,
		public NinePatch
{
	std::shared_ptr<ResNinePatch> activeNinePatch;
	std::shared_ptr<ResNinePatch> inactiveNinePatch;
	
public:
	Tab(const stob::Node* chain);
	
	Tab(const Tab&) = delete;
	Tab& operator=(const Tab&) = delete;
	
	void onCheckedChanged() override;
	
	bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId) override;

	void onHoverChanged(unsigned pointerID) override{}

	
};
}
