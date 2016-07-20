#include "Slider.hpp"

#include "../../App.hpp"
#include "../../util/util.hpp"
#include "../core/proxy/MouseProxy.hpp"
#include "../NinePatch.hpp"



using namespace morda;



namespace{

const char* DDescription = R"qwertyuiop(
		NinePatch{
			name{morda_slider_bg}
			layout{
				dx{max} dy{max}
			}
		}
		Container{
			layout{
				dx{max} dy{max}
			}
			Frame{
				name{morda_handle}
				NinePatch{
					name{morda_handle_image}

					layout{
						dx{max} dy{max}
					}
				}
				MouseProxy{
					name{morda_handle_proxy}
					layout{
						dx{fill} dy{fill}
					}
				}
			}
		}
	)qwertyuiop";

}



HandleSlider::HandleSlider(bool isVertical, const stob::Node* chain) :
		Widget(chain),
		Frame(stob::parse(DDescription).get()),
		handle(*this->findChildByName("morda_handle")),
		isVertical(isVertical)
{
	{
		auto np = this->findChildByNameAs<NinePatch>("morda_slider_bg");
		ASSERT(np)
		if(auto n = getProperty(chain, "background")){
			np->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>(n->value()));
		}else{
			np->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>("morda_npt_slider_bg"));
		}
	}
	
	{
		auto hi = this->findChildByNameAs<NinePatch>("morda_handle_image");
		ASSERT(hi)
		if(auto n = getProperty(chain, "handleNinePatch")){
			hi->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>(n->value()));
		}else{
			hi->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>("morda_npt_slider_handle"));
		}
	}
	
	auto hp = this->findChildByNameAs<MouseProxy>("morda_handle_proxy");
	hp->mouseButton = [this](Widget& widget, bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId) -> bool{
		if(button != MouseButton_e::LEFT){
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

		unsigned longIndex = this->getLongIndex();

		float maxPos = this->rect().d[longIndex] - this->handle.rect().d[longIndex];
		utki::clampBottom(maxPos, 0.0f);

		float newPos = this->handle.rect().p[longIndex];
		newPos += pos[longIndex] - this->clickPoint;
		utki::clampRange(newPos, 0.0f, maxPos);

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
	this->Frame::layOut();
	
	unsigned longIndex = this->getLongIndex();
	
	morda::Vec2r newSize(this->rect().d);
	
	newSize[longIndex] = ::round(newSize[longIndex] * this->areaSizeFactor());
	utki::clampBottom(newSize[longIndex], this->measure(Vec2r(-1))[longIndex]);
	
	this->handle.resize(newSize);
	
	//move
	{
		float effectiveLength = this->rect().d[longIndex] - this->handle.rect().d[longIndex];
		morda::Vec2r newPos(0);
		if(effectiveLength > 0){
			if(this->isVertical){
				newPos[longIndex] = this->rect().d.y - this->handle.rect().d.y - ::round(effectiveLength * this->factor());
			}else{
				newPos[longIndex] = ::round(effectiveLength * this->factor());
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
