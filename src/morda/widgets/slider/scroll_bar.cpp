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
#include "../label/nine_patch.hpp"
#include "../proxy/mouse_proxy.hpp"

using namespace morda;

namespace {

const auto layout_description = treeml::read(R"qwertyuiop(
		layout{pile}

		@nine_patch{
			id{morda_slider_bg}
			lp{
				dx{max} dy{max}
			}
		}
		@container{
			lp{
				dx{max} dy{max}
			}
			@pile{
				id{morda_handle}
				@nine_patch{
					id{morda_handle_image}

					lp{
						dx{max} dy{max}
					}
				}
				@mouse_proxy{
					id{morda_handle_proxy}
					lp{
						dx{fill} dy{fill}
					}
				}
			}
		}
	)qwertyuiop");

} // namespace

scroll_bar::scroll_bar(const utki::shared_ref<morda::context>& c, const treeml::forest& desc, bool vertical) :
	widget(c, desc),
	fraction_band_widget(this->context, treeml::forest()),
	oriented(vertical),
	container(this->context, layout_description),
	handle(this->get_widget("morda_handle"))
{
	auto np = this->try_get_widget_as<nine_patch>("morda_slider_bg");
	ASSERT(np)

	auto hi = this->try_get_widget_as<nine_patch>("morda_handle_image");
	ASSERT(hi)

	bool background_set = false;
	bool handle_set = false;

	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "background") {
			np->set_nine_patch(
				this->context.get().loader.load<res::nine_patch>(get_property_value(p).to_string()).to_shared_ptr()
			);
			background_set = true;
		} else if (p.value == "nine_patch_of_handle") {
			hi->set_nine_patch(
				this->context.get().loader.load<res::nine_patch>(get_property_value(p).to_string()).to_shared_ptr()
			);
			handle_set = true;
		}
	}

	if (!background_set) {
		np->set_nine_patch(this->context.get().loader.load<res::nine_patch>("morda_npt_slider_bg").to_shared_ptr());
	}

	if (!handle_set) {
		hi->set_nine_patch(this->context.get().loader.load<res::nine_patch>("morda_npt_slider_handle").to_shared_ptr());
	}

	auto hp = this->try_get_widget_as<mouse_proxy>("morda_handle_proxy");
	hp->mouse_button_handler = [this](mouse_proxy&, const mouse_button_event& e) -> bool {
		if (e.button != mouse_button::left) {
			return false;
		}

		if (e.is_down) {
			ASSERT(!this->isGrabbed)
			this->isGrabbed = true;

			unsigned long_index = this->get_long_index();
			this->clickPoint = e.pos[long_index];

			return true;
		} else {
			if (this->isGrabbed) {
				this->isGrabbed = false;
				return true;
			} else {
				return false;
			}
		}
	};

	hp->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) -> bool {
		if (!this->isGrabbed) {
			return false;
		}

		using std::min;
		using std::max;

		unsigned long_index = this->get_long_index();

		float max_pos = this->rect().d[long_index] - this->handle.rect().d[long_index];
		max_pos = max(max_pos, 0.0f); // clamp bottom

		float new_pos = this->handle.rect().p[long_index];
		new_pos += e.pos[long_index] - this->clickPoint;
		new_pos = max(new_pos, real(0)); // clamp bottom
		new_pos = min(new_pos, max_pos); // clamp top

		ASSERT(0 <= new_pos && new_pos <= max_pos, [&](auto& o) {
			o << "new_pos = " << new_pos << ", max_pos = " << max_pos;
		})

		morda::vector2 new_position(0);
		new_position[long_index] = new_pos;

		this->handle.move_to(new_position);

		ASSERT(max_pos >= 0)

		if (max_pos > 0) {
			// update factor
			this->set_fraction(new_pos / max_pos);
		}

		return true;
	};
}

void scroll_bar::on_fraction_change()
{
	this->lay_out();

	this->fraction_widget::on_fraction_change();
}

void scroll_bar::on_lay_out()
{
	this->container::on_lay_out();

	unsigned long_index = this->get_long_index();
	unsigned trans_index = this->get_trans_index();

	morda::vector2 new_size(this->rect().d);

	using std::round;

	new_size[long_index] = round(new_size[long_index] * this->get_band_fraction());

	auto min_handle_size = this->handle.measure(vector2(-1));

	using std::max;
	new_size[long_index] =
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
		max(new_size[long_index], std::round(real(1.5) * min_handle_size[trans_index])); // clamp bottom

	this->handle.resize(new_size);

	// move scroll handle
	{
		float effective_length = this->rect().d[long_index] - this->handle.rect().d[long_index];
		morda::vector2 new_pos(0);
		if (effective_length > 0) {
			new_pos[long_index] = round(effective_length * this->fraction());
			ASSERT(new_pos[long_index] <= effective_length, [&](auto& o) {
				o << "new_pos[long_index] = " << new_pos[long_index] << ", effective_length = " << effective_length
				  << ", this->factor() = " << this->fraction();
			})
		}
		this->handle.move_to(new_pos);
	}
}

void scroll_bar::on_band_change()
{
	this->lay_out();

	this->fraction_band_widget::on_band_change();
}
