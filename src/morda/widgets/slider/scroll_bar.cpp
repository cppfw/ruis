/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "scroll_bar.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

#include "../proxy/mouse_proxy.hpp"

#include "../label/nine_patch.hpp"

using namespace morda;

namespace{

const auto DDescription = treeml::read(R"qwertyuiop(
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

scroll_bar::scroll_bar(const utki::shared_ref<morda::context>& c, const treeml::forest& desc, bool vertical) :
		widget(c, desc),
		fraction_band_widget(this->context, treeml::forest()),
		oriented_widget(this->context, treeml::forest(), vertical),
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
			np->set_nine_patch(this->context.get().loader.load<res::nine_patch>(get_property_value(p).to_string()).to_shared_ptr());
			background_set = true;
		}else if(p.value == "nine_patch_of_handle"){
			hi->set_nine_patch(this->context.get().loader.load<res::nine_patch>(get_property_value(p).to_string()).to_shared_ptr());
			handle_set = true;
		}
	}

	if(!background_set){
		np->set_nine_patch(this->context.get().loader.load<res::nine_patch>("morda_npt_slider_bg").to_shared_ptr());
	}

	if(!handle_set){
		hi->set_nine_patch(this->context.get().loader.load<res::nine_patch>("morda_npt_slider_handle").to_shared_ptr());
	}

	auto hp = this->try_get_widget_as<mouse_proxy>("morda_handle_proxy");
	hp->mouse_button_handler = [this](mouse_proxy&, const mouse_button_event& e) -> bool{
		if(e.button != mouse_button::left){
			return false;
		}

		if(e.is_down){
			ASSERT(!this->isGrabbed)
			this->isGrabbed = true;

			unsigned longIndex = this->get_long_index();
			this->clickPoint = e.pos[longIndex];

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

	hp->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) -> bool {
		if(!this->isGrabbed){
			return false;
		}

		using std::min;
		using std::max;

		unsigned longIndex = this->get_long_index();

		float maxPos = this->rect().d[longIndex] - this->handle.rect().d[longIndex];
		maxPos = max(maxPos, 0.0f); // clamp bottom

		float newPos = this->handle.rect().p[longIndex];
		newPos += e.pos[longIndex] - this->clickPoint;
		newPos = max(newPos, real(0)); // clamp bottom
		newPos = min(newPos, maxPos); // clamp top

		ASSERT(
			0 <= newPos && newPos <= maxPos,
			[&](auto&o){o << "newPos = " << newPos << ", maxPos = " << maxPos;}
		)

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

	using std::max;
	newSize[longIndex] = max(newSize[longIndex], std::round(real(1.5) * minHandleSize[transIndex])); // clamp bottom

	this->handle.resize(newSize);

	// move scroll handle
	{
		float effectiveLength = this->rect().d[longIndex] - this->handle.rect().d[longIndex];
		morda::vector2 newPos(0);
		if(effectiveLength > 0){
			newPos[longIndex] = ::round(effectiveLength * this->fraction());
			ASSERT(
				newPos[longIndex] <= effectiveLength,
				[&](auto&o){
					o << "newPos[longIndex] = " << newPos[longIndex]
					<< ", effectiveLength = " << effectiveLength
					<< ", this->factor() = " << this->fraction();
				}
			)
		}
		this->handle.move_to(newPos);
	}
}

void scroll_bar::on_band_change(){
	this->lay_out();

	this->fraction_band_widget::on_band_change();
}
