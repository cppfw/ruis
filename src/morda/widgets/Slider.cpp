#include "Slider.hpp"

#include "../App.hpp"
#include "../util/util.hpp"
#include "MouseProxy.hpp"

#include <ting/math.hpp>
#include <ting/util.hpp>


using namespace morda;



namespace{

const char* DDescription = R"qwertyuiop(
		NinePatch{
			image{morda_npt_slider}
			layout{
				dimX{max} dimY{max}
			}
		}
		Container{
			layout{
				dimX{max} dimY{max}
			}
			FrameContainer{
				name{handle}
				NinePatch{
					image{morda_npt_slider_handle}

					layout{
						dimX{max} dimY{max}
					}
				}
				MouseProxy{
					name{morda_handle_proxy}
					layout{
						dimX{max} dimY{max}
					}
				}
			}
		}
	)qwertyuiop";

}



Slider::Slider(bool isVertical, const stob::Node* chain) :
		Widget(chain),
		FrameContainer(stob::Parse(DDescription).get()),
		handle(*this->FindChildByName("handle")),
		isVertical(isVertical)
{
	auto hp = this->FindChildByNameAs<MouseProxy>("morda_handle_proxy");
	hp->onMouseButton = [this](Widget& widget, bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId) -> bool{
		if(button != EMouseButton::LEFT){
			return false;
		}

		if(isDown){
			ASSERT(!this->isGrabbed)
			this->isGrabbed = true;

			unsigned longIndex = this->isVertical ? 1 : 0;
			this->clickPoint = pos[longIndex];

			return true;
		}else{
			if(this->isGrabbed){
				this->isGrabbed = false;
				return true;
			}else{
				return false;
			}
		}
	};
	
	hp->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId) -> bool{
		if(!this->isGrabbed){
			return false;
		}

		unsigned longIndex = this->GetLongIndex();

		float maxPos = this->Rect().d[longIndex] - this->handle.Rect().d[longIndex];
		ASSERT(maxPos >= 0)

		float newPos = this->handle.Rect().p[longIndex];
		newPos += pos[longIndex] - this->clickPoint;
		ting::util::ClampRange(newPos, 0.0f, maxPos);

		morda::Vec2r newPosition(0);
		newPosition[longIndex] = newPos;

		//update factor
		if(this->isVertical){
			this->curFactor = (maxPos - newPos) / maxPos;
		}else{
			
			this->curFactor = newPos / maxPos;
		}
		
		this->handle.MoveTo(newPosition);

		if(this->factorChange){
			this->factorChange(*this);
		}

		return true;
	};
}



void Slider::SetFactor(float newFactor){
	real factor = ting::util::ClampedRange(newFactor, 0.0f, 1.0f);
	
	if(this->curFactor == factor){
		return;
	}
	
	this->curFactor = factor;
	
	this->Slider::layOut();
	
	if(this->factorChange){
		this->factorChange(*this);
	}
}



void Slider::layOut(){
	this->FrameContainer::layOut();
	
	unsigned longIndex = this->GetLongIndex();
	
	morda::Vec2r newSize(this->Rect().d);
	
	newSize[longIndex] = ting::math::Round(newSize[longIndex] * this->handleSizeFactor);
	ting::util::ClampBottom(newSize[longIndex], this->measure(Vec2r(-1))[longIndex]);
	
	this->handle.Resize(newSize);
	
	//move
	{
		float effectiveLength = this->Rect().d[longIndex] - this->handle.Rect().d[longIndex];
		morda::Vec2r newPos(0);
		if(effectiveLength > 0){
			if(this->isVertical){
				newPos[longIndex] = this->Rect().d.y - this->handle.Rect().d.y - ting::math::Round(effectiveLength * this->curFactor);
			}else{
				newPos[longIndex] = ting::math::Round(effectiveLength * this->curFactor);
			}
			ASSERT(newPos[longIndex] <= effectiveLength)
		}
		this->handle.MoveTo(newPos);
	}
}



morda::Vec2r Slider::onMeasure(const morda::Vec2r& quotum)const noexcept{
	Vec2r ret = quotum;
	for(unsigned i = 0; i != ret.size(); ++i){
		if(ret[i] < 0){
			ret[i] = 30; //TODO: what should it be???
		}
	}
	return ret;
}

	