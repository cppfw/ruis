/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Widget.hpp"


namespace morda{

class MouseProxy : virtual public Widget{
public:
	MouseProxy(const stob::Node* chain = nullptr);
	
	MouseProxy(const MouseProxy&) = delete;
	MouseProxy& operator=(const MouseProxy&) = delete;
	
	std::function<bool (Widget& widget, bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)> mouseButton;
	
	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)override;
	
	std::function<bool (Widget& widget, const morda::Vec2r& pos, unsigned pointerId)> mouseMove;
	
	bool onMouseMove(const morda::Vec2r& pos, unsigned pointerId)override;
	
	std::function<void(Widget& widget, unsigned pointerID)> hoverChanged;
	
	void onHoverChanged(unsigned pointerID)override;
};

}
