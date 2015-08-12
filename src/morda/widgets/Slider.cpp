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

		float maxPos = this->rect().d[longIndex] - this->handle.rect().d[longIndex];
		ASSERT(maxPos >= 0)

		float newPos = this->handle.rect().p[longIndex];
		newPos += pos[longIndex] - this->clickPoint;
		ting::util::ClampRange(newPos, 0.0f, maxPos);

		morda::Vec2r newPosition(0);
		newPosition[longIndex] = newPos;

		this->handle.MoveTo(newPosition);
		
		//update factor
		if(this->isVertical){
			this->setFactor((maxPos - newPos) / maxPos);
		}else{
			this->setFactor(newPos / maxPos);
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
	
	this->handle.Resize(newSize);
	
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
			ASSERT(newPos[longIndex] <= effectiveLength)
		}
		this->handle.MoveTo(newPos);
	}
}



morda::Vec2r HandleSlider::onMeasure(const morda::Vec2r& quotum)const noexcept{
	Vec2r ret = quotum;
	for(unsigned i = 0; i != ret.size(); ++i){
		if(ret[i] < 0){
			ret[i] = 30; //TODO: what should it be???
		}
	}
	return ret;
}

	