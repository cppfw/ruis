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

#include "scroll_area.hpp"

#include "../../context.hpp"
#include "../../layout/measure_mode.hpp"
#include "../../util/util.hpp"

using namespace ruis;

scroll_area::scroll_area(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	utki::shared_ref<ruis::widget> child
) :
	widget(
		context, //
		std::move(params.layout_params),
		[&]() {
			if (auto& c = params.widget.clip; !c.has_value()) {
				c = true;
			}

			return std::move(params.widget);
		}()
	),
	// clang-format off
	container(
		context,
		{},
		{
			std::move(child)
		}
	)
// clang-format on
{}

ruis::vec2 scroll_area::measure(
	const ruis::vec2& quotum, //
	const r4::vector2<measure_mode>& mode //
) const
{
	// In at_most dimensions, report the size needed to show all the contents without scrolling.
	vec2 ret(quotum);
	if (mode.x() == measure_mode::at_most || mode.y() == measure_mode::at_most) {
		vec2 d = dims_for_widget(
			this->child(), //
			quotum,
			mode
		);
		for (unsigned i = 0; i != ret.size(); ++i) {
			if (mode[i] == measure_mode::at_most) {
				ret[i] = d[i];
			}
		}
	}
	return ret;
}

event_status scroll_area::on_mouse_button(const mouse_button_event& e)
{
	vec2 d = -this->cur_scroll_pos;
	return this->front().get().on_mouse_button(mouse_button_event{
		.action = e.action, //
		.pos = e.pos - d,
		.button = e.button,
		.pointer_id = e.pointer_id
	});
}

event_status scroll_area::on_mouse_move(const mouse_move_event& e)
{
	vec2 d = -this->cur_scroll_pos;
	return this->front().get().on_mouse_move(mouse_move_event{
		.pos = e.pos - d, //
		.pointer_id = e.pointer_id,
		.ignore_mouse_capture = e.ignore_mouse_capture
	});
}

void scroll_area::render(const ruis::mat4& matrix) const
{
	vec2 d = -this->cur_scroll_pos;

	mat4 matr(matrix);
	matr.translate(d);

	this->render_child(matr, this->child());
}

void scroll_area::clamp_scroll_pos()
{
	using std::max;
	using std::min;

	utki::assert(this->invisible_dims.is_positive_or_zero(), SL);

	// std::cout << "scroll_area::clamp_scroll_pos(): invisible_dims: " << this->invisible_dims << "\n";
	// std::cout << "scroll_area::clamp_scroll_pos(): before clamping this->cur_scroll_pos: " << this->cur_scroll_pos << "\n";

	this->cur_scroll_pos =
		max(real(0), //
			min(this->cur_scroll_pos, //
				this->invisible_dims));
}

void scroll_area::set_scroll_pos(const vec2& new_scroll_pos)
{
	using std::round;

	// std::cout << "sceoll_area::set_scroll_pos(): this->cur_scroll_pos: " << this->cur_scroll_pos << "\n";

	this->cur_scroll_pos = round(new_scroll_pos);

	this->clamp_scroll_pos();

	// std::cout << "sceoll_area::set_scroll_pos(): after clamping this->cur_scroll_pos: " << this->cur_scroll_pos << "\n";

	this->update_scroll_factor();

	this->on_scroll_pos_change();
}

vec2 scroll_area::scroll_by(const vec2& delta)
{
	auto old_scroll_pos = this->get_scroll_pos();
	this->set_scroll_pos(old_scroll_pos + delta);
	return this->get_scroll_pos() - old_scroll_pos;
}

void scroll_area::set_scroll_factor(const vec2& factor)
{
	auto clamped_factor =
		max(real(0), //
			min(factor, //
				real(1)));
	vec2 new_scroll_pos = this->invisible_dims.comp_mul(clamped_factor);

	this->set_scroll_pos(new_scroll_pos);
}

void scroll_area::update_scroll_factor()
{
	// at this point effective dimension should be updated
	vec2 factor = this->cur_scroll_pos.comp_div(this->invisible_dims);

	if (this->cur_scroll_factor == factor) {
		return;
	}

	for (unsigned i = 0; i != 2; ++i) {
		if (this->invisible_dims[i] == 0) {
			this->cur_scroll_factor[i] = 0;
		} else {
			utki::assert(this->invisible_dims[i] > 0, SL);
			this->cur_scroll_factor[i] = this->cur_scroll_pos[i] / this->invisible_dims[i];
		}
	}
}

void scroll_area::on_lay_out()
{
	auto& child = this->front().get();

	// Position the child at the top left corner and size it according to its layout parameters:
	// 'fill' and 'max' dimensions match the scroll area size (so there is no scrolling in that
	// direction), 'min' and 'undefined' dimensions use the child's minimal (natural) size,
	// 'length' dimensions use the fixed size.
	child.move_to(vec2(0));
	child.resize(dims_for_widget(
		child, //
		this->rect().d, //
		r4::vector2<measure_mode>(measure_mode::exactly)
	));

	this->update_invisible_dims();

	// correct scroll position

	// distance of content's bottom right corner from bottom right corner of the scroll_area
	vec2 br = this->cur_scroll_pos - this->invisible_dims;

	for (size_t i = 0; i != br.size(); ++i) {
		if (br[i] > 0) {
			if (br[i] <= this->cur_scroll_pos[i]) {
				this->cur_scroll_pos[i] -= br[i];
			} else {
				this->cur_scroll_pos[i] = 0;
			}
		}
	}

	// TODO: why notification is deferred? figure out why and write a comment with explanation here
	// UPDATE: perhaps because it invoked scroll_change_handler() and doing it during layout perhaps is not a good time.
	this->context.get().post_to_ui_thread([sa = utki::make_weak_from(*this)]() {
		if (auto s = sa.lock()) {
			s->on_scroll_pos_change();
		}
	});
}

void scroll_area::update_invisible_dims()
{
	using std::max;

	this->invisible_dims = max(this->front().get().rect().d - this->rect().d, vec2(0));
	this->update_scroll_factor();
}

vec2 scroll_area::get_visible_area_fraction() const noexcept
{
	auto ret = this->rect().d.comp_div(this->rect().d + this->invisible_dims);

	using std::min;
	ret = min(ret, real(1)); // clamp top

	return ret;
}

void scroll_area::on_scroll_pos_change()
{
	if (this->scroll_change_handler) {
		this->scroll_change_handler(*this);
	}
}

utki::shared_ref<ruis::scroll_area> ruis::make::scroll_area(
	const utki::shared_ref<ruis::context>& context, //
	ruis::scroll_area::all_parameters params,
	utki::shared_ref<ruis::widget> child
)
{
	return utki::make_shared<ruis::scroll_area>(
		context, //
		std::move(params),
		std::move(child)
	);
}
