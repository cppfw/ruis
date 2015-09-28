/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Widget.hpp"
#include "containers/FrameContainer.hpp"


//disable stupid warnings
#if M_COMPILER == M_COMPILER_MSVC
#	pragma warning(disable : 4250)
#endif


namespace morda{


class Slider : public virtual Widget{
	float curFactor = 0; //Current position from 0 to 1
protected:
	Slider(const stob::Node* chain = nullptr) :
			Widget(chain)
	{}
	
	virtual void onFactorChange(){
		if(this->factorChange){
			this->factorChange(*this);
		}
	}
public:
	std::function<void(Slider&)> factorChange;
	
	float factor()const noexcept{
		return this->curFactor;
	}
	
	void setFactor(float newFactor){
		real factor = ting::util::ClampedRange(newFactor, 0.0f, 1.0f);
	
		//in case of nan or inf
		if(!(0 <= factor && factor <= 1)){
			TRACE(<< "factor = " << factor << std::endl)
			throw morda::IllegalValueExc("Factor should be in [0:1]");
		}
		
		ASSERT_INFO(0 <= factor && factor <= 1, factor)
		
		if(this->curFactor == factor){
			return;
		}

		this->curFactor = factor;
		
		this->onFactorChange();
	}
};


class AreaSlider : public Slider{
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
		private FrameContainer //users do not need to know that it is a container
{
	//no copying
	HandleSlider(const HandleSlider&);
	HandleSlider& operator=(const HandleSlider&);
	
	unsigned GetLongIndex()const noexcept{
		return this->isVertical ? 1 : 0;
	}

	unsigned GetTransIndex()const noexcept{
		return this->isVertical ? 0 : 1;
	}
	
	Widget& handle;
	
	bool isVertical;
	
	bool isGrabbed = false;
	float clickPoint;
	
protected:
	HandleSlider(bool isVertical, const stob::Node* chain);

	virtual void onFactorChange();

public:
	

	virtual ~HandleSlider()noexcept{}
	
private:
	void layOut() override;
	
	morda::Vec2r measure(const morda::Vec2r& quotum)const noexcept override;
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

}//~namespace
