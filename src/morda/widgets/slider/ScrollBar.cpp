#include "ScrollBar.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

#include "../proxy/MouseProxy.hpp"

#include "../label/NinePatch.hpp"



using namespace morda;



namespace{

const auto DDescription = puu::read(R"qwertyuiop(
		NinePatch{
			id{morda_slider_bg}
			layout{
				dx{max} dy{max}
			}
		}
		Container{
			layout{
				dx{max} dy{max}
			}
			Pile{
				id{morda_handle}
				NinePatch{
					id{morda_handle_image}

					layout{
						dx{max} dy{max}
					}
				}
				MouseProxy{
					id{morda_handle_proxy}
					layout{
						dx{fill} dy{fill}
					}
				}
			}
		}
	)qwertyuiop");

}



ScrollBar::ScrollBar(const std::shared_ptr<morda::context>& c, const puu::forest& desc, bool vertical) :
		widget(c, desc),
		FractionBandWidget(c, puu::forest()),
		OrientedWidget(c, puu::forest(), vertical),
		Pile(c, DDescription),
		handle(*this->try_get_widget("morda_handle")) //TODO: use non-try version
{
	auto np = this->try_get_widget_as<NinePatch>("morda_slider_bg");
	ASSERT(np)

	auto hi = this->try_get_widget_as<NinePatch>("morda_handle_image");
	ASSERT(hi)

	bool background_set = false;
	bool handle_set = false;
	
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "background"){
			np->setNinePatch(this->context->loader.load<ResNinePatch>(get_property_value(p).to_string()));
			background_set = true;
		}else if(p.value == "handleNinePatch"){
			hi->setNinePatch(this->context->loader.load<ResNinePatch>(get_property_value(p).to_string()));
			handle_set = true;
		}
	}

	if(!background_set){
		np->setNinePatch(this->context->loader.load<ResNinePatch>("morda_npt_slider_bg"));
	}

	if(!handle_set){
		hi->setNinePatch(this->context->loader.load<ResNinePatch>("morda_npt_slider_handle"));
	}

	auto hp = this->try_get_widget_as<MouseProxy>("morda_handle_proxy");
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

	hp->mouseMove = [this](morda::widget& widget, const morda::Vec2r& pos, unsigned pointerId) -> bool{
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

		this->handle.move_to(newPosition);

		ASSERT(maxPos >= 0)

		if(maxPos > 0){
			//update factor
			this->setFraction(newPos / maxPos);
		}

		return true;
	};
}



void ScrollBar::onFractionChange() {
	this->lay_out();

	this->FractionWidget::onFractionChange();
}



void ScrollBar::lay_out(){
	this->Pile::lay_out();

	unsigned longIndex = this->getLongIndex();
	unsigned transIndex = this->getTransIndex();

	morda::Vec2r newSize(this->rect().d);

	newSize[longIndex] = ::round(newSize[longIndex] * this->bandSizeFraction());

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
		this->handle.move_to(newPos);
	}
}


void ScrollBar::onBandSizeChanged(){
	this->lay_out();

	this->FractionBandWidget::onBandSizeChanged();
}
