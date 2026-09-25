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

#include "text_input.hpp"

#include "../../context.hpp"
#include "../../util/key.hpp"
#include "../../util/util.hpp"

#if M_OS == M_OS_WINDOWS
#	ifdef DELETE
#		undef DELETE
#	endif
#endif

using namespace ruis;

namespace {
const uint32_t cursor_blink_period = 500; // milliseconds

const real cursor_width = real(1.0);
} // namespace

text_input::text_input(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params,
	string text
) :
	widget(
		context, //
		std::move(params.layout_params),
		std::move(params.widget)
	),
	text_string_widget(
		context, //
		std::move(params.params.text),
		std::move(text)
	),
	character_input_widget(context),
	params([&]() {
		if (params.params.specific.hint_color.get().is_undefined()) {
			params.params.specific.hint_color = context.get().style().get_color_text_secondary();
		}
		return std::move(params.params.specific);
	}())
{
	this->set_clip(true);
}

void text_input::render(const ruis::mat4& matrix) const
{
	// render selection
	if (this->cursor_index != this->selection_start_index) {
		ruis::mat4 matr(matrix);
		matr.translate(
			this->selection_start_index < this->cursor_index ? this->selection_start_pos : this->cursor_pos,
			0
		);

		using std::abs;
		matr.scale(vec2(
			abs(this->cursor_pos - this->selection_start_pos), //
			this->rect().d.y()
		));

		auto& r = this->context.get().renderer.get();

		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			this->text_string_widget::get_params().selection_color.get()
		);
	}

	// render text or hint
	if (this->get_string().get().empty()) {
		// render hint
		ruis::mat4 matr(matrix);

		using std::round;

		const auto& font = this->get_font();

		matr.translate(
			-this->get_bounding_box().p.x() + this->x_offset,
			round((font.get_height() + font.get_ascender() - font.get_descender()) / 2)
		);

		font.render(
			this->ctx().ren(), //
			matr,
			this->params.hint_color.get(),
			this->params.hint.get()
		);
	} else {
		// render text
		ruis::mat4 matr(matrix);

		using std::round;

		const auto& font = this->get_font();

		matr.translate(
			-this->get_bounding_box().p.x() + this->x_offset,
			round((font.get_height() + font.get_ascender() - font.get_descender()) / 2)
		);

		utki::assert(this->first_visible_char_index <= this->get_string().get().size());
		font.render(
			this->ctx().ren(), //
			matr,
			this->get_color(),
			this->get_string().get().substr(
				this->first_visible_char_index, //
				this->get_string().get().size() - this->first_visible_char_index
			)
		);
	}

	// render cursor
	if (this->is_focused() && this->cursor_blink_visible) {
		ruis::mat4 matr(matrix);
		matr.translate(this->cursor_pos, 0);
		matr.scale(vec2(cursor_width * this->context.get().units.dots_per_fp(), this->rect().d.y()));

		auto& r = this->context.get().renderer.get();
		r.shaders().color_pos->render(
			matr, //
			r.obj().pos_quad_01_vao.get(),
			this->get_color()
		);
	}
}

event_status text_input::on_mouse_button(const mouse_button_event& e)
{
	if (e.button != mouse_button::left) {
		return event_status::propagate;
	}

	this->left_mouse_button_down = (e.action == button_action::press);

	if (e.action == button_action::press) {
		this->set_cursor_index(this->pos_to_index(e.pos.x()));
	}

	return event_status::consumed;
}

event_status text_input::on_mouse_move(const mouse_move_event& e)
{
	if (!this->left_mouse_button_down) {
		return event_status::propagate;
	}

	this->set_cursor_index(this->pos_to_index(e.pos.x()), true);
	return event_status::consumed;
}

vec2 text_input::measure(const ruis::vec2& quotum) const noexcept
{
	vec2 ret;

	if (quotum.x() < 0) {
		ret.x() = this->get_bounding_box().d.x() + cursor_width * this->context.get().units.dots_per_fp();
	} else {
		ret.x() = quotum.x();
	}

	if (quotum.y() < 0) {
		ret.y() = this->get_font().get_height();
	} else {
		ret.y() = quotum.y();
	}

	return ret;
}

void text_input::set_cursor_index(
	size_t index, //
	bool selection
)
{
	this->cursor_index = index;

	using std::min;
	this->cursor_index = min(this->cursor_index, this->get_string().get().size()); // clamp top

	if (!selection) {
		this->selection_start_index = this->cursor_index;
	}

	//	TRACE(<< "selection_start_index = " << this->selection_start_index << std::endl)

	this->update_cursor_pos_based_on_index();

	this->selection_start_pos = this->index_to_pos(this->selection_start_index);

	if (!this->is_focused()) {
		this->focus();
	}
	this->start_cursor_blinking();
}

void text_input::update_cursor_pos_based_on_index()
{
	if (this->cursor_index <= this->first_visible_char_index) {
		this->first_visible_char_index = this->cursor_index;
		this->x_offset = 0;
		this->cursor_pos = 0;
		return;
	}

	const auto& font = this->get_font();

	utki::assert(this->first_visible_char_index <= this->get_string().get().size());
	utki::assert(this->cursor_index > this->first_visible_char_index);
	this->cursor_pos = font.get_advance(std::u32string(
						   this->get_string().get(),
						   this->first_visible_char_index,
						   this->cursor_index - this->first_visible_char_index
					   )) +
		this->x_offset;

	utki::assert(this->cursor_pos >= 0);

	if (this->cursor_pos > this->rect().d.x() - cursor_width * this->context.get().units.dots_per_fp()) {
		this->cursor_pos = this->rect().d.x() - cursor_width * this->context.get().units.dots_per_fp();

		this->x_offset = this->cursor_pos; // start from rightmost cursor position
		this->first_visible_char_index = this->cursor_index;

		// calculate advance backwards
		for (auto i =
				 utki::next(this->get_string().get().rbegin(), this->get_string().get().size() - this->cursor_index);
			 this->x_offset > 0;
			 ++i)
		{
			utki::assert(i != this->get_string().get().rend());
			this->x_offset -= font.get_advance(*i);
			utki::assert(this->first_visible_char_index > 0);
			--this->first_visible_char_index;
		}
	}
}

void text_input::on_text_change()
{
	text_string_widget::on_text_change();

	using std::min;
	const auto size = this->get_string().get().size();

	this->cursor_index = min(this->cursor_index, size);
	this->selection_start_index = min(this->selection_start_index, size);
	this->first_visible_char_index = min(this->first_visible_char_index, size);

	this->update_cursor_pos_based_on_index();
	this->selection_start_pos = this->index_to_pos(this->selection_start_index);
}

real text_input::index_to_pos(size_t index)
{
	utki::assert(this->first_visible_char_index <= this->get_string().get().size());

	if (index <= this->first_visible_char_index) {
		return 0;
	}

	using std::min;
	index = min(index, this->get_string().get().size()); // clamp top

	real ret = this->x_offset;

	for (auto i = utki::next(this->get_string().get().begin(), this->first_visible_char_index);
		 i != this->get_string().get().end() && index != this->first_visible_char_index;
		 ++i, --index)
	{
		ret += this->get_font().get_advance(*i);
		if (ret >= this->rect().d.x()) {
			ret = this->rect().d.x();
			break;
		}
	}

	return ret;
}

size_t text_input::pos_to_index(real pos)
{
	size_t index = this->first_visible_char_index;
	real p = this->x_offset;

	for (auto i = utki::next(this->get_string().get().begin(), this->first_visible_char_index);
		 i != this->get_string().get().end();
		 ++i)
	{
		real w = this->get_font().get_advance(*i);

		if (pos < p + w) {
			if (pos < p + w / 2) {
				break;
			}
			++index;
			break;
		}

		p += w;
		++index;
	}

	return index;
}

void text_input::update(uint32_t dt)
{
	this->cursor_blink_visible = !this->cursor_blink_visible;
}

void text_input::on_focus_change()
{
	if (this->is_focused()) {
		this->ctrl_pressed = false;
		this->shift_pressed = false;
		this->start_cursor_blinking();
	} else {
		this->context.get().updater.get().stop(*this);
	}
	this->context.get().window().set_virtual_keyboard_visible(this->is_focused());
}

void text_input::on_resize()
{
	this->selection_start_pos = this->index_to_pos(this->selection_start_index);
}

void text_input::start_cursor_blinking()
{
	this->context.get().updater.get().stop(*this);
	this->cursor_blink_visible = true;
	this->context.get().updater.get().start(
		utki::make_shared_from(*static_cast<updateable*>(this)), //
		cursor_blink_period
	);
}

event_status text_input::on_key(const ruis::key_event& e)
{
	switch (e.combo.key) {
		case ruis::key::left_control:
		case ruis::key::right_control:
			this->ctrl_pressed = (e.action == button_action::press);
			break;
		case ruis::key::left_shift:
		case ruis::key::right_shift:
			this->shift_pressed = (e.action == button_action::press);
			break;
		default:
			break;
	}
	return event_status::propagate;
}

void text_input::on_character_input(const character_input_event& e)
{
	switch (e.combo.key) {
		case ruis::key::enter:
			break;
		case ruis::key::arrow_right:
			if (this->cursor_index != this->get_string().get().size()) {
				size_t new_index = 0;
				if (this->ctrl_pressed) {
					bool space_skipped = false;
					new_index = this->cursor_index;
					for (auto i = utki::next(this->get_string().get().begin(), this->cursor_index);
						 i != this->get_string().get().end();
						 ++i, ++new_index)
					{
						if (*i == uint32_t(' ')) {
							if (space_skipped) {
								break;
							}
						} else {
							space_skipped = true;
						}
					}

				} else {
					new_index = this->cursor_index + 1;
				}
				this->set_cursor_index(new_index, this->shift_pressed);
			}
			break;
		case ruis::key::arrow_left:
			if (this->cursor_index != 0) {
				size_t new_index = 0;
				if (this->ctrl_pressed) {
					bool space_skipped = false;
					new_index = this->cursor_index;
					for (auto i = utki::next(
							 this->get_string().get().rbegin(),
							 this->get_string().get().size() - this->cursor_index
						 );
						 i != this->get_string().get().rend();
						 ++i, --new_index)
					{
						if (*i == uint32_t(' ')) {
							if (space_skipped) {
								break;
							}
						} else {
							space_skipped = true;
						}
					}
				} else {
					new_index = this->cursor_index - 1;
				}
				this->set_cursor_index(new_index, this->shift_pressed);
			}
			break;
		case ruis::key::end:
			this->set_cursor_index(this->get_string().get().size(), this->shift_pressed);
			break;
		case ruis::key::home:
			this->set_cursor_index(0, this->shift_pressed);
			break;
		case ruis::key::backspace:
			if (this->there_is_selection()) {
				this->set_cursor_index(this->delete_selection());
			} else {
				if (this->cursor_index != 0) {
					auto t = this->get_string().get();
					t.erase(utki::next(t.begin(), this->cursor_index - 1));
					this->set_string_no_notify(std::move(t));
					this->set_cursor_index(this->cursor_index - 1);
				}
			}
			break;
		case ruis::key::deletion:
			if (this->there_is_selection()) {
				this->set_cursor_index(this->delete_selection());
			} else {
				if (this->cursor_index < this->get_string().get().size()) {
					auto t = this->get_string().get();
					t.erase(utki::next(t.begin(), this->cursor_index));
					this->set_string_no_notify(std::move(t));
					this->set_cursor_index(this->cursor_index);
				}
			}
			this->start_cursor_blinking();
			break;
		case ruis::key::escape:
			// do nothing
			break;
		case ruis::key::a:
			if (this->ctrl_pressed) {
				this->selection_start_index = 0;
				this->set_cursor_index(this->get_string().get().size(), true);
				break;
			}
			// fall through
		default:
			if (!e.string.empty()) {
				size_t replace_start = this->cursor_index;
				size_t replace_end = this->cursor_index;
				if (this->there_is_selection()) {
					replace_start = std::min(this->cursor_index, this->selection_start_index);
					replace_end = std::max(this->cursor_index, this->selection_start_index);
				}

				const auto& original = this->get_string().get();
				if (!this->params.filter || this->params.filter(original, replace_start, replace_end, e.string)) {
					if (this->there_is_selection()) {
						this->cursor_index = this->delete_selection();
					}

					auto t = this->get_string().get();
					t.insert(utki::next(t.begin(), this->cursor_index), e.string.begin(), e.string.end());
					this->set_string_no_notify(std::move(t));

					this->set_cursor_index(this->cursor_index + e.string.size());
				}
			}

			break;
	}
}

size_t text_input::delete_selection()
{
	utki::assert(this->cursor_index != this->selection_start_index);

	size_t start = 0;
	size_t end = 0;
	if (this->cursor_index < this->selection_start_index) {
		start = this->cursor_index;
		end = this->selection_start_index;
	} else {
		start = this->selection_start_index;
		end = this->cursor_index;
	}

	auto t = this->get_string().get();
	t.erase(utki::next(t.begin(), start), utki::next(t.begin(), end));
	this->set_string_no_notify(std::move(t));

	return start;
}

utki::shared_ref<ruis::text_input> ruis::make::text_input(
	const utki::shared_ref<ruis::context>& context, //
	ruis::text_input::all_parameters params,
	ruis::string text
)
{
	return utki::make_shared<ruis::text_input>(
		context, //
		std::move(params),
		std::move(text)
	);
}