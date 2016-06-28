/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "Container.hpp"


namespace morda{



class LinearContainer : public Container{
	LinearContainer(const LinearContainer&) = delete;
	LinearContainer& operator=(const LinearContainer&) = delete;

	bool isVertical;
	
	unsigned GetLongIndex()const noexcept{
		return this->isVertical ? 1 : 0;
	}

	unsigned GetTransIndex()const noexcept{
		return this->isVertical ? 0 : 1;
	}
	
protected:
	LinearContainer(bool isVertical, const stob::Node* chain);
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



class Vertical : public LinearContainer{
public:
	Vertical(const stob::Node* chain = nullptr) :
			Widget(chain),
			LinearContainer(true, chain)
	{}
	
	Vertical(const Vertical&) = delete;
	Vertical& operator=(const Vertical&) = delete;
};



class Horizontal : public LinearContainer{
public:
	Horizontal(const stob::Node* chain = nullptr) :
			Widget(chain),
			LinearContainer(false, chain)
	{}
	
	Horizontal(const Horizontal&) = delete;
	Horizontal& operator=(const Horizontal&) = delete;
};


}
