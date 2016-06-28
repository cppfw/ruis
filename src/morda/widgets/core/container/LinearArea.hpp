/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "Container.hpp"


namespace morda{



class LinearArea : public Container{
	LinearArea(const LinearArea&) = delete;
	LinearArea& operator=(const LinearArea&) = delete;

	bool isVertical;
	
	unsigned GetLongIndex()const noexcept{
		return this->isVertical ? 1 : 0;
	}

	unsigned GetTransIndex()const noexcept{
		return this->isVertical ? 0 : 1;
	}
	
protected:
	LinearArea(bool isVertical, const stob::Node* chain);
public:

	void layOut() override;	
	
	morda::Vec2r measure(const morda::Vec2r& quotum)const override;
	
	
	class LayoutParams : public Container::LayoutParams{
	public:
		LayoutParams(const stob::Node* chain = nullptr);
		
		real weight;
	};
private:
	std::unique_ptr<Widget::LayoutParams> createLayoutParams(const stob::Node* chain)const override{
		return utki::makeUnique<LayoutParams>(chain);
	}
};



class VerticalArea : public LinearArea{
public:
	VerticalArea(const stob::Node* chain = nullptr) :
			Widget(chain),
			LinearArea(true, chain)
	{}
	
	VerticalArea(const VerticalArea&) = delete;
	VerticalArea& operator=(const VerticalArea&) = delete;
};



class HorizontalArea : public LinearArea{
public:
	HorizontalArea(const stob::Node* chain = nullptr) :
			Widget(chain),
			LinearArea(false, chain)
	{}
	
	HorizontalArea(const HorizontalArea&) = delete;
	HorizontalArea& operator=(const HorizontalArea&) = delete;
};


}
