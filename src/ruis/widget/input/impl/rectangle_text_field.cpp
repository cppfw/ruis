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

#include "rectangle_text_field.hpp"

#include <utki/debug.hpp>

using namespace ruis;

using namespace ruis::length_literals;

rectangle_text_field::rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	ruis::string text
) :
	rectangle_text_field(
		context,
		params,
		// clang-format off
		ruis::make::text_input(
			context, //
			{
				.layout_params{
					.dims = {ruis::dim::max, ruis::dim::max}
				},
				.params = std::move(params.params.text_input)
			},
			std::move(text)
		)
		// clang-format on
	)
{}

rectangle_text_field::rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters& params,
	utki::shared_ref<ruis::text_input> text_input
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget)
	),
	// Initialize rectangle first so it adds the text_input as a child
	// clang-format off
	rectangle(
		context,
		{
			.params = [&](){
				if(auto& l = params.params.rectangle.padding.container.layout; !l){
					l = layout::pile;
				}

				for(auto& b : params.params.rectangle.padding.specific.borders){
					if(b.get().is_undefined()){
						b = context.get().style().get_len_gap();
					}
				}

				for(auto& r : params.params.rectangle.specific.corner_radii){
					if(r.get().is_undefined()){
						r  = context.get().style().get_len_gap_small();
					}
				}

				if(auto& c = params.params.rectangle.specific.fill_color; c.get().is_undefined()){
					c = context.get().style().get_color_background();
				}
				if(auto& c = params.params.rectangle.specific.stroke_color; c.get().is_undefined()){
					c = context.get().style().get_color_primary();
				}
				if(auto& l = params.params.rectangle.specific.stroke_width; l.get().is_undefined()){
					l = context.get().style().get_len_border();
				}

				return std::move(params.params.rectangle);
			}()
		},
		{
			text_input
		}
	),
	// clang-format on
	text_field(
		context, //
		text_input.get()
	)
{}

event_status rectangle_text_field::on_mouse_button(const mouse_button_event& e)
{
	// The text_input is laid out inside the rectangle's padding, so a mouse event
	// landing in the padding area would not reach the text_input (the base container
	// only forwards events to children whose rectangle overlaps the event position).
	// Forward the event to the text_input directly, clamping the mouse position to the
	// text_input's rectangle (in the text_input's local coordinates).
	mouse_button_event clamped = e;
	clamped.pos = this->clamp_pos_to_text_input(e.pos);
	return this->get_text_input().on_mouse_button(clamped);
}

event_status rectangle_text_field::on_mouse_move(const mouse_move_event& e)
{
	mouse_move_event clamped = e;
	clamped.pos = this->clamp_pos_to_text_input(e.pos);
	return this->get_text_input().on_mouse_move(clamped);
}

vec2 rectangle_text_field::clamp_pos_to_text_input(const vec2& pos)
{
	// The text_input is a descendant of this widget (it is wrapped in the padding's
	// inner content container, which is offset by the borders), so its rect() is
	// expressed in the content container's coordinates, not in this widget's
	// coordinates. Compute the text_input's origin in this widget's coordinates, clamp
	// the given position to the text_input's rectangle in this widget's coordinates,
	// and return the result in the text_input's local coordinates.
	auto& ti = this->get_text_input();
	const vec2 ti_origin = ti.get_pos_in_ancestor(vec2(0, 0), this);
	const vec2 ti_size = ti.rect().d;

	const vec2 lo = ti_origin;
	const vec2 hi = ti_origin + ti_size;

	using std::min;
	using std::max;
	vec2 clamped;
	clamped.x() = max(min(pos.x(), hi.x()), lo.x());
	clamped.y() = max(min(pos.y(), hi.y()), lo.y());

	return clamped - ti_origin;
}

utki::shared_ref<ruis::rectangle_text_field> ruis::make::rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::rectangle_text_field::all_parameters params,
	ruis::string text
)
{
	return utki::make_shared<ruis::rectangle_text_field>(
		context, //
		std::move(params),
		std::move(text)
	);
}