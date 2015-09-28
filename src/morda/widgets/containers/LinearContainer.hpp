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
		LayoutParams(const stob::Node* chain);
		
		real weight;
		
		static std::unique_ptr<LayoutParams> New(const stob::Node* chain = nullptr){
			return std::unique_ptr<LayoutParams>(new LayoutParams(chain));
		}
	};
private:
	std::unique_ptr<Widget::LayoutParams> createLayoutParams(const stob::Node* chain)const override{
		return LayoutParams::New(chain);
	}
};



class VerticalContainer : public LinearContainer{
public:
	VerticalContainer(const stob::Node* chain = nullptr) :
			Widget(chain),
			LinearContainer(true, chain)
	{}
	
	VerticalContainer(const VerticalContainer&) = delete;
	VerticalContainer& operator=(const VerticalContainer&) = delete;
};



class HorizontalContainer : public LinearContainer{
public:
	HorizontalContainer(const stob::Node* chain = nullptr) :
			Widget(chain),
			LinearContainer(false, chain)
	{}
	
	HorizontalContainer(const HorizontalContainer&) = delete;
	HorizontalContainer& operator=(const HorizontalContainer&) = delete;
};


}
