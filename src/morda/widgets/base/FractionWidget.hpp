#pragma once

#include "../Widget.hpp"

namespace morda{
class FractionWidget : public virtual Widget{
	float fraction_v = 0; //Current position from 0 to 1
protected:
	FractionWidget(const stob::Node* chain = nullptr) :
			Widget(chain)
	{}
	
	virtual void onFractionChange(){
		if(this->fractionChange){
			this->fractionChange(*this);
		}
	}
public:
	std::function<void(FractionWidget&)> fractionChange;
	
	real fraction()const noexcept{
		return this->fraction_v;
	}
	
	void setFraction(float newFraction){
		real fraction = utki::clampedRange(newFraction, 0.0f, 1.0f);
	
		//in case of nan or inf
		if(!(0 <= fraction && fraction <= 1)){
			fraction = 0;
		}
		
		ASSERT_INFO(0 <= fraction && fraction <= 1, fraction)
		
		if(this->fraction_v == fraction){
			return;
		}

		this->fraction_v = fraction;
		
		this->onFractionChange();
	}
};
}