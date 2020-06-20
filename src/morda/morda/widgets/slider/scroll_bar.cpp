#include "scroll_bar.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

#include "../proxy/mouse_proxy.hpp"

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

scroll_bar::scroll_bar(std::shared_ptr<morda::context> c, const puu::forest& desc, bool vertical) :
		widget(std::move(c), desc),
		fraction_band_widget(this->context, puu::forest()),
		oriented_widget(this->context, puu::forest(), vertical),
		pile(this->context, DDescription),
		handle(this->get_widget("morda_handle"))
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
			np->set_nine_patch(this->context->loader.load<res::nine_patch>(get_property_value(p).to_string()));
			background_set = true;
		}else if(p.value == "nine_patch_of_handle"){
			hi->set_nine_patch(this->context->loader.load<res::nine_patch>(get_property_value(p).to_string()));
			handle_set = true;
		}
	}

	if(!background_set){
		np->set_nine_patch(this->context->loader.load<res::nine_patch>("morda_npt_slider_bg"));
	}

	if(!handle_set){
		hi->set_nine_patch(this->context->loader.load<res::nine_patch>("morda_npt_slider_handle"));
	}

	auto hp = this->try_get_widget_as<mouse_proxy>("morda_handle_proxy");
	hp->mouse_button_handler = [this](widget& widget, bool isDown, const morda::vector2& pos, mouse_button button, unsigned pointerId) -> bool{
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

	hp->mouse_move_handler = [this](morda::widget& widget, const morda::vector2& pos, unsigned pointerId) -> bool{
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

		morda::vector2 newPosition(0);
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

void scroll_bar::on_fraction_change() {
	this->lay_out();

	this->fraction_widget::on_fraction_change();
}

void scroll_bar::lay_out(){
	this->pile::lay_out();

	unsigned longIndex = this->get_long_index();
	unsigned transIndex = this->get_trans_index();

	morda::vector2 newSize(this->rect().d);

	newSize[longIndex] = ::round(newSize[longIndex] * this->get_band_fraction());

	auto minHandleSize = this->handle.measure(vector2(-1));

	utki::clampBottom(newSize[longIndex], std::round(real(1.5) * minHandleSize[transIndex]));

	this->handle.resize(newSize);

	// move scroll handle
	{
		float effectiveLength = this->rect().d[longIndex] - this->handle.rect().d[longIndex];
		morda::vector2 newPos(0);
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

void scroll_bar::on_band_changed(){
	this->lay_out();

	this->fraction_band_widget::on_band_changed();
}
