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



HandleSlider::HandleSlider(bool isVertical, const stob::Node* chain) :
		Widget(chain),
		FrameContainer(stob::Parse(DDescription).get()),
		handle(*this->findChildByName("handle")),
		isVertical(isVertical)
{
	auto hp = this->findChildByNameAs<MouseProxy>("morda_handle_proxy");
	hp->mouseButton = [this](Widget& widget, bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId) -> bool{
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
	
	hp->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId) -> bool{
		if(!this->isGrabbed){
			return false;
		}

		unsigned longIndex = this->GetLongIndex();

		float maxPos = this->rect().d[longIndex] - this->handle.rect().d[longIndex];
		ting::util::ClampBottom(maxPos, 0.0f);

		float newPos = this->handle.rect().p[longIndex];
		newPos += pos[longIndex] - this->clickPoint;
		ting::util::ClampRange(newPos, 0.0f, maxPos);

		ASSERT_INFO(0 <= newPos && newPos <= maxPos, "newPos = " << newPos << ", maxPos = " << maxPos)
		
		morda::Vec2r newPosition(0);
		newPosition[longIndex] = newPos;

		this->handle.moveTo(newPosition);
		
		ASSERT(maxPos >= 0)
				
		if(maxPos > 0){
			//update factor
			if(this->isVertical){
				this->setFactor((maxPos - newPos) / maxPos);
			}else{
				this->setFactor(newPos / maxPos);
			}
		}

		return true;
	};
}



void HandleSlider::onFactorChange() {
	this->HandleSlider::layOut();
	
	this->Slider::onFactorChange();
}



void HandleSlider::layOut(){
	this->FrameContainer::layOut();
	
	unsigned longIndex = this->GetLongIndex();
	
	morda::Vec2r newSize(this->rect().d);
	
	newSize[longIndex] = ting::math::Round(newSize[longIndex] * this->areaSizeFactor());
	ting::util::ClampBottom(newSize[longIndex], this->measure(Vec2r(-1))[longIndex]);
	
	this->handle.resize(newSize);
	
	//move
	{
		float effectiveLength = this->rect().d[longIndex] - this->handle.rect().d[longIndex];
		morda::Vec2r newPos(0);
		if(effectiveLength > 0){
			if(this->isVertical){
				newPos[longIndex] = this->rect().d.y - this->handle.rect().d.y - ting::math::Round(effectiveLength * this->factor());
			}else{
				newPos[longIndex] = ting::math::Round(effectiveLength * this->factor());
			}
			ASSERT_INFO(
					newPos[longIndex] <= effectiveLength,
					"newPos[longIndex] = " << newPos[longIndex]
							<< ", effectiveLength = " << effectiveLength
							<< ", this->factor() = " << this->factor()
				)
		}
		this->handle.moveTo(newPos);
	}
}



morda::Vec2r HandleSlider::measure(const morda::Vec2r& quotum)const noexcept{
	Vec2r ret = quotum;
	for(unsigned i = 0; i != ret.size(); ++i){
		if(ret[i] < 0){
			ret[i] = 30; //TODO: what should it be???
		}
	}
	return ret;
}

	