#pragma once

#include "../core/Widget.hpp"

#include "../core/container/Pile.hpp"
#include "../base/FractionWidget.hpp"

//disable stupid warnings
#if M_COMPILER == M_COMPILER_MSVC
#	pragma warning(disable : 4250)
#endif


namespace morda{


class AreaSlider : public FractionWidget{
	float curAreaSizeFactor = 0; //Current area size factor from 0 to 1
protected:
	AreaSlider(const stob::Node* chain = nullptr) :
			Widget(chain)
	{}
	
public:
	
	real areaSizeFactor()const noexcept{
		return this->curAreaSizeFactor;
	}
	
	//TODO:
};


class HandleSlider :
		public AreaSlider,
		private Pile //users do not need to know that it is a container
{
	//no copying
	HandleSlider(const HandleSlider&);
	HandleSlider& operator=(const HandleSlider&);
	
	unsigned getLongIndex()const noexcept{
		return this->isVertical ? 1 : 0;
	}

	unsigned getTransIndex()const noexcept{
		return this->isVertical ? 0 : 1;
	}
	
	Widget& handle;
	
	bool isVertical;
	
	bool isGrabbed = false;
	float clickPoint;
	
protected:
	HandleSlider(bool isVertical, const stob::Node* chain);

	virtual void onFractionChange() override;

public:

	virtual ~HandleSlider()noexcept{}
	
private:
	void layOut() override;
};



class VerticalSlider : public HandleSlider{
public:
	VerticalSlider(const stob::Node* chain = nullptr) : 
			Widget(chain),
			HandleSlider(true, chain)
	{}
	
	VerticalSlider(const VerticalSlider&) = delete;
	VerticalSlider& operator=(const VerticalSlider&) = delete;
};


class HorizontalSlider : public HandleSlider{
public:
	HorizontalSlider(const stob::Node* chain = nullptr) : 
			Widget(chain),
			HandleSlider(false, chain)
	{}
	
	HorizontalSlider(const HorizontalSlider&) = delete;
	HorizontalSlider& operator=(const HorizontalSlider&) = delete;
};

}
