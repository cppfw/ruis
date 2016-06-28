/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Container.hpp"

#include <functional>


namespace morda{



class ScrollArea : public Container{
	//offset from top left corner
	Vec2r curScrollPos = Vec2r(0);
	
	//cached effectiveDim
	Vec2r effectiveDim;
	
	//cached scroll factor
	Vec2r curScrollFactor;
	
public:
	ScrollArea(const stob::Node* chain = nullptr);
	
	ScrollArea(const ScrollArea&) = delete;
	ScrollArea& operator=(const ScrollArea&) = delete;
	

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID)override;
	
	bool onMouseMove(const morda::Vec2r& pos, unsigned pointerID)override;
	
	void render(const morda::Matr4r& matrix) const override;

	morda::Vec2r measure(const morda::Vec2r& quotum) const override{
		return this->Widget::measure(quotum);
	}

	void layOut()override;

	void onChildrenListChanged()override;
	
	const Vec2r& scrollPos()const{
		return this->curScrollPos;
	}
	
	
	void setScrollPos(const Vec2r& newScrollPos);
	
	void setScrollPosAsFactor(const Vec2r& factor);
	
	const Vec2r& scrollFactor()const{
		return this->curScrollFactor;
	}
	
private:
	void updateEffectiveDim();
	
	void updateScrollFactor();

	void clampScrollPos();
	
	void arrangeWidgets();
};



}
