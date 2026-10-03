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

namespace {

/**
 * @brief Layout of the scroll area's content container.
 * The child widgets keep their positions and are resized according to their layout parameters:
 * 'fill' and 'max' children are stretched to the container size, 'min' children are assigned
 * their minimal size and 'length' children are assigned the fixed length.
 * In 'at_most' measure mode the layout reports the minimal size needed to fit all the child
 * widgets (their positions are taken into account).
 */
// TODO: no need for special layout when scroll_area will not inherit container.
class scroll_content_layout : public ruis::layout
{
public:
	ruis::vec2 measure(
		const ruis::vec2& quotum, //
		const r4::vector2<ruis::measure_mode>& mode, //
		ruis::const_widget_list& widgets
	) const override
	{
		ruis::vec2 ret;
		for (unsigned i = 0; i != ret.size(); ++i) {
			ret[i] = (mode[i] == ruis::measure_mode::exactly) ? quotum[i] : ruis::real(0);
		}

		for (const auto& w : widgets) {
			auto& ww = w.get();
			const auto& lp = ww.get_layout_params_const();

			// A child with fill dims in both directions does not constrain the content's min size:
			// it fills whatever size the container is given, so there is no point in measuring it.
			if (lp.dims[0].get_type() == ruis::dim::type::fill && lp.dims[1].get_type() == ruis::dim::type::fill) {
				continue;
			}

			ruis::vec2 d;
			r4::vector2<ruis::measure_mode> m;
			for (unsigned i = 0; i != 2; ++i) {
				const auto& dim = lp.dims[i];
				switch (dim.get_type()) {
					case ruis::dim::type::fill:
						d[i] = 0;
						m[i] = ruis::measure_mode::exactly;
						break;
					case ruis::dim::type::undefined:
						[[fallthrough]];
					case ruis::dim::type::min:
						[[fallthrough]];
					case ruis::dim::type::max:
						d[i] = ruis::measure_infinite_quotum;
						m[i] = ruis::measure_mode::at_most;
						break;
					case ruis::dim::type::length:
						d[i] = dim.get_length().get(ww.context);
						m[i] = ruis::measure_mode::exactly;
						break;
				}
			}

			if (m[0] == ruis::measure_mode::at_most || m[1] == ruis::measure_mode::at_most) {
				ruis::vec2 md = ww.measure(d, m);
				for (unsigned i = 0; i != md.size(); ++i) {
					if (m[i] == ruis::measure_mode::at_most) {
						d[i] = md[i];
					}
				}
			}

			for (unsigned i = 0; i != ret.size(); ++i) {
				// fill dims do not constrain the content's min size
				if (mode[i] == ruis::measure_mode::at_most && lp.dims[i].get_type() != ruis::dim::type::fill) {
					using std::max;
					ret[i] = max(ret[i], ww.rect().p[i] + d[i]);
				}
			}
		}

		return ret;
	}

	void lay_out(
		const ruis::vec2& dims, //
		ruis::semiconst_widget_list& widgets
	) const override
	{
		for (const auto& w : widgets) {
			auto& ww = w.get();
			ww.resize(ruis::dims_for_widget(
				ww, //
				dims, //
				r4::vector2<ruis::measure_mode>(ruis::measure_mode::exactly)
			));
		}
	}
};

// the layout instance used for scroll area content containers
const utki::shared_ref<ruis::layout> scroll_content_layout_inst = utki::make_shared<scroll_content_layout>();

} // namespace

scroll_area::scroll_area(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	widget_list children
) :
	scroll_area(
		context, //
		params,
		// clang-format off
		make::container(
			context,
			{
				.params{
					.layout = scroll_content_layout_inst
				}
			},
			std::move(children)
		)
		// clang-format on
	)
{}

scroll_area::scroll_area(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters& params,
	utki::shared_ref<ruis::container> content_container
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget)
	),
	// clang-format off
	container(
		context,
		{},
		{
			content_container
		}
	),
	// clang-format on
	containing_widget(
		context, //
		content_container.get()
	),
	content_container(std::move(content_container))
{
	this->get_container().move_to({0, 0});
}

event_status scroll_area::on_mouse_button(const mouse_button_event& e)
{
	vec2 d = -this->cur_scroll_pos;
	return this->get_container().on_mouse_button(mouse_button_event{
		.action = e.action, //
		.pos = e.pos - d,
		.button = e.button,
		.pointer_id = e.pointer_id
	});
}

event_status scroll_area::on_mouse_move(const mouse_move_event& e)
{
	vec2 d = -this->cur_scroll_pos;
	return this->get_container().on_mouse_move(mouse_move_event{
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

	this->get_container().render(matr);
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
	auto& cont = this->get_container();

	if(!cont.is_layout_dirty()){
		return;
	}

	// measure minimal dims of the content container
	vec2 min_dims = cont.measure(
		vec2(measure_infinite_quotum), //
		r4::vector2<measure_mode>(measure_mode::at_most)
	);

	// resize the content container to fit all its contents while covering the whole visible area
	// TODO: why covering the whole visible area? If the container is smaller than scroll_area then it is fine.
	using std::max;
	vec2 cont_dims = max(this->rect().d, min_dims);
	cont.move_to(vec2(0));
	cont.resize(cont_dims);

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

	this->invisible_dims = max(this->get_container().rect().d - this->rect().d, vec2(0));
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
	ruis::widget_list children
)
{
	return utki::make_shared<ruis::scroll_area>(
		context, //
		std::move(params),
		std::move(children)
	);
}
