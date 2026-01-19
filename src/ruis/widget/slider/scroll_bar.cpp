/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include <utki/config.hpp>

#include "../../context.hpp"
#include "../../util/util.hpp"
#include "../label/nine_patch.hpp"
#include "../proxy/mouse_proxy.hpp"

using namespace std::string_literals;

using namespace ruis;

namespace m {
using namespace ruis::make;
} // namespace m

namespace {
std::vector<utki::shared_ref<ruis::widget>> make_widgets(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return {
		m::nine_patch(c,
			{
				.layout_params = {
					.dims = {ruis::dim::max, ruis::dim::max}
				},
				.widget_params = {
					.id = "ruis_background"s
				}
			}
		),
		m::container(c,
			{
				.layout_params = {
					.dims = {ruis::dim::max, ruis::dim::max}
				}
			},
			{
				m::pile(c,
					{
						.widget_params = {
							.id = "ruis_handle"s
						}
					},
					{
						m::nine_patch(c,
							{
								.layout_params = {
									.dims = {ruis::dim::max, ruis::dim::max}
								},
								.widget_params = {
									.id = "ruis_handle_image"s
								}
							}
						),
						m::mouse_proxy(c,
							{
								.layout_params = {
									.dims = {ruis::dim::fill, ruis::dim::fill}
								},
								.widget_params = {
									.id = "ruis_handle_proxy"s
								}
							}
						)
					}
				)
			}
		)
	};
	// clang-format on
}
} // namespace

scroll_bar::scroll_bar( //
	utki::shared_ref<ruis::context> c,
	all_parameters params
) :
	widget( //
		std::move(c),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	fraction_band_widget( //
		this->context,
		{
			.fraction_params = std::move(params.fraction_params),
			.fraction_band_params = std::move(params.fraction_band_params) //
		}
	),
	oriented(std::move(params.oriented_params)),
	container( //
		this->context,
		{.container_params = {.layout = layout::pile}},
		make_widgets(this->context)
	),
	handle(this->get_widget("ruis_handle")),
	params([&]() {
		auto& sbp = params.scroll_bar_params;
		if (!sbp.background) {
			sbp.background = this->context.get().loader().load<res::nine_patch>("ruis_npt_slider_bg").to_shared_ptr();
		}
		if (!sbp.handle) {
			sbp.handle = this->context.get().loader().load<res::nine_patch>("ruis_npt_slider_handle").to_shared_ptr();
		}
		return params.scroll_bar_params;
	}())
{
	auto& np = this->get_widget_as<nine_patch>("ruis_background");
	auto& hi = this->get_widget_as<nine_patch>("ruis_handle_image");

	np.set_nine_patch(this->params.background);
	hi.set_nine_patch(this->params.handle);

	auto hp = this->try_get_widget_as<mouse_proxy>("ruis_handle_proxy");
	hp->mouse_button_handler = [this](mouse_proxy&, const mouse_button_event& e) -> bool {
		if (e.button != mouse_button::left) {
			return false;
		}

		if (e.is_down) {
			ASSERT(!this->is_grabbed)
			this->is_grabbed = true;

			unsigned long_index = this->get_long_index();
			this->grab_point = e.pos[long_index];

			return true;
		} else {
			if (this->is_grabbed) {
				this->is_grabbed = false;
				return true;
			} else {
				return false;
			}
		}
	};

	hp->mouse_move_handler = [this](mouse_proxy&, const mouse_move_event& e) -> bool {
		if (!this->is_grabbed) {
			return false;
		}

		using std::min;
		using std::max;

		unsigned long_index = this->get_long_index();

		float max_pos = this->rect().d[long_index] - this->handle.rect().d[long_index];
		max_pos = max(max_pos, 0.0f); // clamp bottom

		float new_pos = this->handle.rect().p[long_index];
		new_pos += e.pos[long_index] - this->grab_point;
		new_pos = max(new_pos, real(0)); // clamp bottom
		new_pos = min(new_pos, max_pos); // clamp top

		ASSERT(0 <= new_pos && new_pos <= max_pos, [&](auto& o) {
			o << "new_pos = " << new_pos << ", max_pos = " << max_pos;
		})

		ruis::vec2 new_position(0);
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
	this->fraction_widget::on_fraction_change();

	this->lay_out();
}

void scroll_bar::on_lay_out()
{
	this->container::on_lay_out();

	unsigned long_index = this->get_long_index();
	unsigned trans_index = this->get_trans_index();

	ruis::vec2 new_size(this->rect().d);

	using std::round;

	new_size[long_index] = round(new_size[long_index] * this->get_band_fraction());

	auto min_handle_size = this->handle.measure(vec2(-1));

	using std::max;
	new_size[long_index] =
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
		max(new_size[long_index], round(real(1.5) * min_handle_size[trans_index])); // clamp bottom

	this->handle.resize(new_size);

	// move scroll handle
	{
		float effective_length = this->rect().d[long_index] - this->handle.rect().d[long_index];
		ruis::vec2 new_pos(0);
		if (effective_length > 0) {
			new_pos[long_index] = round(effective_length * this->get_fraction());
			ASSERT(new_pos[long_index] <= effective_length, [&](auto& o) {
				o << "new_pos[long_index] = " << new_pos[long_index] << ", effective_length = " << effective_length
				  << ", this->factor() = " << this->get_fraction();
			})
		}
		this->handle.move_to(new_pos);
	}
}

void scroll_bar::on_band_change()
{
	this->fraction_band_widget::on_band_change();

	this->lay_out();
}
