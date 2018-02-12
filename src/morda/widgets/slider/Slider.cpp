#include "Slider.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"

#include "../core/proxy/MouseProxy.hpp"

#include "../label/NinePatch.hpp"



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
			Pile{
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
		Pile(stob::parse(DDescription).get()),
		handle(*this->findByName("morda_handle")),
		isVertical(isVertical)
{
	{
		auto np = this->findByNameAs<NinePatch>("morda_slider_bg");
		ASSERT(np)
		if(auto n = getProperty(chain, "background")){
			np->setNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>(n->value()));
		}else{
			np->setNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>("morda_npt_slider_bg"));
		}
	}
	
	{
		auto hi = this->findByNameAs<NinePatch>("morda_handle_image");
		ASSERT(hi)
		if(auto n = getProperty(chain, "handleNinePatch")){
			hi->setNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>(n->value()));
		}else{
			hi->setNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>("morda_npt_slider_handle"));
		}
	}
	
	auto hp = this->findByNameAs<MouseProxy>("morda_handle_proxy");
	hp->mouseButton = [this](Widget& widget, bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId) -> bool{
		if(button != MouseButton_e::LEFT){
			return false;
		}

		if(isDown){
			ASSERT(!this->isGrabbed)
			this->isGrabbed = true;

			unsigned longIndex = this->getLongIndex();
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
			this->setFraction(newPos / maxPos);
		}

		return true;
	};
}



void HandleSlider::onFractionChange() {
	this->layOut();
	
	this->FractionWidget::onFractionChange();
}



void HandleSlider::layOut(){
	this->Pile::layOut();
	
	unsigned longIndex = this->getLongIndex();
	unsigned transIndex = this->getTransIndex();
	
	morda::Vec2r newSize(this->rect().d);
	
	newSize[longIndex] = ::round(newSize[longIndex] * this->areaSizeFraction());
	
	auto minHandleSize =  this->handle.measure(Vec2r(-1));
	
	utki::clampBottom(newSize[longIndex], std::round(real(1.5) * minHandleSize[transIndex]));
	
	this->handle.resize(newSize);
	
	//move
	{
		float effectiveLength = this->rect().d[longIndex] - this->handle.rect().d[longIndex];
		morda::Vec2r newPos(0);
		if(effectiveLength > 0){
			newPos[longIndex] = ::round(effectiveLength * this->fraction());
			ASSERT_INFO(
					newPos[longIndex] <= effectiveLength,
					"newPos[longIndex] = " << newPos[longIndex]
							<< ", effectiveLength = " << effectiveLength
							<< ", this->factor() = " << this->fraction()
				)
		}
		this->handle.moveTo(newPos);
	}
}

void AreaSlider::onAreaSizeChanged() {
	if (this->areaSizeChanged) {
		this->areaSizeChanged(*this);
	}
}


void HandleSlider::onAreaSizeChanged() {
	this->layOut();
	
	this->AreaSlider::onAreaSizeChanged();
}


void AreaSlider::setAreaSizeFraction(real fraction) {
	if (this->curAreaSizeFraction == fraction) {
		return;
	}

	this->curAreaSizeFraction = fraction;
	this->onAreaSizeChanged();
}
