#include "ScrollBar.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

#include "../proxy/MouseProxy.hpp"

#include "../label/nine_patch.hpp"

using namespace morda;

namespace{

const auto DDescription = puu::read(R"qwertyuiop(
		@nine_patch{
			id{morda_slider_bg}
			layout{
				dx{max} dy{max}
			}
		}
		@container{
			layout{
				dx{max} dy{max}
			}
			@pile{
				id{morda_handle}
				@nine_patch{
					id{morda_handle_image}

					layout{
						dx{max} dy{max}
					}
				}
				@mouse_proxy{
					id{morda_handle_proxy}
					layout{
						dx{fill} dy{fill}
					}
				}
			}
		}
	)qwertyuiop");

}

ScrollBar::ScrollBar(std::shared_ptr<morda::context> c, const puu::forest& desc, bool vertical) :
		widget(std::move(c), desc),
		FractionBandWidget(this->context, puu::forest()),
		oriented_widget(this->context, puu::forest(), vertical),
		pile(this->context, DDescription),
		handle(*this->try_get_widget("morda_handle")) //TODO: use non-try version
{
	auto np = this->try_get_widget_as<nine_patch>("morda_slider_bg");
	ASSERT(np)

	auto hi = this->try_get_widget_as<nine_patch>("morda_handle_image");
	ASSERT(hi)

	bool background_set = false;
	bool handle_set = false;
	
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "background"){
			np->set_nine_patch(this->context->loader.load<res_nine_patch>(get_property_value(p).to_string()));
			background_set = true;
		}else if(p.value == "handleNinePatch"){
			hi->set_nine_patch(this->context->loader.load<res_nine_patch>(get_property_value(p).to_string()));
			handle_set = true;
		}
	}

	if(!background_set){
		np->set_nine_patch(this->context->loader.load<res_nine_patch>("morda_npt_slider_bg"));
	}

	if(!handle_set){
		hi->set_nine_patch(this->context->loader.load<res_nine_patch>("morda_npt_slider_handle"));
	}

	auto hp = this->try_get_widget_as<MouseProxy>("morda_handle_proxy");
	hp->mouseButton = [this](widget& widget, bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerId) -> bool{
		if(button != mouse_button::left){
			return false;
		}

		if(isDown){
			ASSERT(!this->isGrabbed)
			this->isGrabbed = true;

			unsigned longIndex = this->get_long_index();
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

		unsigned longIndex = this->get_long_index();

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
			// update factor
			this->set_fraction(newPos / maxPos);
		}

		return true;
	};
}

void ScrollBar::on_fraction_change() {
	this->lay_out();

	this->fraction_widget::on_fraction_change();
}

void ScrollBar::lay_out(){
	this->pile::lay_out();

	unsigned longIndex = this->get_long_index();
	unsigned transIndex = this->get_trans_index();

	morda::Vec2r newSize(this->rect().d);

	newSize[longIndex] = ::round(newSize[longIndex] * this->bandSizeFraction());

	auto minHandleSize = this->handle.measure(Vec2r(-1));

	utki::clampBottom(newSize[longIndex], std::round(real(1.5) * minHandleSize[transIndex]));

	this->handle.resize(newSize);

	// move scroll handle
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
