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

#include "text_input_line.hpp"

#include "../../context.hpp"
#include "../../util/key.hpp"
#include "../../util/util.hpp"

#if M_OS == M_OS_WINDOWS
#	ifdef DELETE
#		undef DELETE
#	endif
#endif

using namespace morda;

namespace {
const uint32_t cursor_blink_period = 500; // milliseconds

const real cursor_width = real(1.0);
} // namespace

text_input_line::text_input_line(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	single_line_text_widget(this->context, desc),
	character_input_widget(this->context),
	color_widget(this->context, desc)
{
	this->set_clip_enabled(true);
}

void text_input_line::render(const morda::matrix4& matrix) const
{
	// render selection
	if (this->cursor_index != this->selection_start_index) {
		morda::matrix4 matr(matrix);
		matr.translate(
			this->selection_start_index < this->cursor_index ? this->selection_start_pos : this->cursor_pos,
			0
		);
		matr.scale(vector2(std::abs(this->cursor_pos - this->selection_start_pos), this->rect().d.y()));

		auto& r = this->context.get().renderer.get();
		r.shader->color_pos->render(matr, r.pos_quad_01_vao.get(), 0xff804040);
	}

	{
		morda::matrix4 matr(matrix);

		using std::round;

		const auto& font = this->get_font().get();

		matr.translate(
			-this->get_bounding_box().p.x() + this->x_offset,
			round((font.get_height() + font.get_ascender() - font.get_descender()) / 2)
		);

		ASSERT(this->first_visible_char_index <= this->get_text().size())
		font.render(
			matr,
			morda::color_to_vec4f(this->get_current_color()),
			this->get_text()
				.substr(this->first_visible_char_index, this->get_text().size() - this->first_visible_char_index)
		);
	}

	if (this->is_focused() && this->cursor_blink_visible) {
		morda::matrix4 matr(matrix);
		matr.translate(this->cursor_pos, 0);
		matr.scale(vector2(cursor_width * this->context.get().units.dots_per_fp, this->rect().d.y()));

		auto& r = this->context.get().renderer.get();
		r.shader->color_pos->render(matr, r.pos_quad_01_vao.get(), this->get_current_color());
	}
}

bool text_input_line::on_mouse_button(const mouse_button_event& e)
{
	if (e.button != mouse_button::left) {
		return false;
	}

	this->left_mouse_button_down = e.is_down;

	if (e.is_down) {
		this->set_cursor_index(this->pos_to_index(e.pos.x()));
	}

	return true;
}

bool text_input_line::on_mouse_move(const mouse_move_event& e)
{
	if (!this->left_mouse_button_down) {
		return false;
	}

	this->set_cursor_index(this->pos_to_index(e.pos.x()), true);
	return true;
}

vector2 text_input_line::measure(const morda::vector2& quotum) const noexcept
{
	vector2 ret;

	if (quotum.x() < 0) {
		ret.x() = this->single_line_text_widget::measure(vector2(-1)).x()
			+ cursor_width * this->context.get().units.dots_per_fp;
	} else {
		ret.x() = quotum.x();
	}

	if (quotum.y() < 0) {
		ret.y() = this->get_font().get().get_height();
	} else {
		ret.y() = quotum.y();
	}

	return ret;
}

void text_input_line::set_cursor_index(size_t index, bool selection)
{
	this->cursor_index = index;

	using std::min;
	this->cursor_index = min(this->cursor_index, this->get_text().size()); // clamp top

	if (!selection) {
		this->selection_start_index = this->cursor_index;
	}

	utki::scope_exit cursor_index_scope_exit([this]() {
		this->selection_start_pos = this->index_to_pos(this->selection_start_index);

		if (!this->is_focused()) {
			this->focus();
		}
		this->start_cursor_blinking();
	});

	//	TRACE(<< "selection_start_index = " << this->selection_start_index << std::endl)

	if (this->cursor_index <= this->first_visible_char_index) {
		this->first_visible_char_index = this->cursor_index;
		this->x_offset = 0;
		this->cursor_pos = 0;
		return;
	}

	const auto& font = this->get_font().get();

	ASSERT(this->first_visible_char_index <= this->get_text().size())
	ASSERT(this->cursor_index > this->first_visible_char_index)
	this->cursor_pos = font.get_advance(std::u32string(
						   this->get_text(),
						   this->first_visible_char_index,
						   this->cursor_index - this->first_visible_char_index
					   ))
		+ this->x_offset;

	ASSERT(this->cursor_pos >= 0)

	if (this->cursor_pos > this->rect().d.x() - cursor_width * this->context.get().units.dots_per_fp) {
		this->cursor_pos = this->rect().d.x() - cursor_width * this->context.get().units.dots_per_fp;

		this->x_offset = this->cursor_pos; // start from rightmost cursor position
		this->first_visible_char_index = this->cursor_index;

		// calculate advance backwards
		for (auto i = utki::next(this->get_text().rbegin(), this->get_text().size() - this->cursor_index);
			 this->x_offset > 0;
			 ++i)
		{
			ASSERT(i != this->get_text().rend())
			this->x_offset -= font.get_advance(*i);
			ASSERT(this->first_visible_char_index > 0)
			--this->first_visible_char_index;
		}
	}
}

real text_input_line::index_to_pos(size_t index)
{
	ASSERT(this->first_visible_char_index <= this->get_text().size())

	if (index <= this->first_visible_char_index) {
		return 0;
	}

	using std::min;
	index = min(index, this->get_text().size()); // clamp top

	real ret = this->x_offset;

	for (auto i = utki::next(this->get_text().begin(), this->first_visible_char_index);
		 i != this->get_text().end() && index != this->first_visible_char_index;
		 ++i, --index)
	{
		ret += this->get_font().get().get_advance(*i);
		if (ret >= this->rect().d.x()) {
			ret = this->rect().d.x();
			break;
		}
	}

	return ret;
}

size_t text_input_line::pos_to_index(real pos)
{
	size_t index = this->first_visible_char_index;
	real p = this->x_offset;

	for (auto i = utki::next(this->get_text().begin(), this->first_visible_char_index); i != this->get_text().end();
		 ++i)
	{
		real w = this->get_font().get().get_advance(*i);

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

void text_input_line::update(uint32_t dt)
{
	this->cursor_blink_visible = !this->cursor_blink_visible;
}

void text_input_line::on_focus_change()
{
	if (this->is_focused()) {
		this->ctrl_pressed = false;
		this->shift_pressed = false;
		this->start_cursor_blinking();
	} else {
		this->context.get().updater.get().stop(*this);
	}
}

void text_input_line::on_resize()
{
	//	TRACE(<< "text_input_line::on_resize(): size = " << this->rect().d << std::endl)
	this->selection_start_pos = this->index_to_pos(this->selection_start_index);
}

void text_input_line::start_cursor_blinking()
{
	this->context.get().updater.get().stop(*this);
	this->cursor_blink_visible = true;
	this->context.get().updater.get().start(
		utki::make_shared_from(*static_cast<updateable*>(this)).to_shared_ptr(),
		cursor_blink_period
	);
}

bool text_input_line::on_key(const morda::key_event& e)
{
	switch (e.combo.key) {
		case morda::key::left_control:
		case morda::key::right_control:
			this->ctrl_pressed = e.is_down;
			break;
		case morda::key::left_shift:
		case morda::key::right_shift:
			this->shift_pressed = e.is_down;
			break;
		default:
			break;
	}
	return false;
}

void text_input_line::on_character_input(const character_input_event& e)
{
	switch (e.combo.key) {
		case morda::key::enter:
			break;
		case morda::key::arrow_right:
			if (this->cursor_index != this->get_text().size()) {
				size_t new_index;
				if (this->ctrl_pressed) {
					bool space_skipped = false;
					new_index = this->cursor_index;
					for (auto i = utki::next(this->get_text().begin(), this->cursor_index); i != this->get_text().end();
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
		case morda::key::arrow_left:
			if (this->cursor_index != 0) {
				size_t new_index;
				if (this->ctrl_pressed) {
					bool space_skipped = false;
					new_index = this->cursor_index;
					for (auto i = utki::next(this->get_text().rbegin(), this->get_text().size() - this->cursor_index);
						 i != this->get_text().rend();
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
		case morda::key::end:
			this->set_cursor_index(this->get_text().size(), this->shift_pressed);
			break;
		case morda::key::home:
			this->set_cursor_index(0, this->shift_pressed);
			break;
		case morda::key::backspace:
			if (this->there_is_selection()) {
				this->set_cursor_index(this->delete_selection());
			} else {
				if (this->cursor_index != 0) {
					auto t = this->get_text();
					this->clear();
					t.erase(utki::next(t.begin(), this->cursor_index - 1));
					this->set_text(std::move(t));
					this->set_cursor_index(this->cursor_index - 1);
				}
			}
			break;
		case morda::key::deletion:
			if (this->there_is_selection()) {
				this->set_cursor_index(this->delete_selection());
			} else {
				if (this->cursor_index < this->get_text().size()) {
					auto t = this->get_text();
					this->clear();
					t.erase(utki::next(t.begin(), this->cursor_index));
					this->set_text(std::move(t));
				}
			}
			this->start_cursor_blinking();
			break;
		case morda::key::escape:
			// do nothing
			break;
		case morda::key::a:
			if (this->ctrl_pressed) {
				this->selection_start_index = 0;
				this->set_cursor_index(this->get_text().size(), true);
				break;
			}
			// fall through
		default:
			if (!e.string.empty()) {
				if (this->there_is_selection()) {
					this->cursor_index = this->delete_selection();
				}

				auto t = this->get_text();
				this->clear();
				t.insert(utki::next(t.begin(), this->cursor_index), e.string.begin(), e.string.end());
				this->set_text(std::move(t));

				this->set_cursor_index(this->cursor_index + e.string.size());
			}

			break;
	}
}

size_t text_input_line::delete_selection()
{
	ASSERT(this->cursor_index != this->selection_start_index)

	size_t start, end;
	if (this->cursor_index < this->selection_start_index) {
		start = this->cursor_index;
		end = this->selection_start_index;
	} else {
		start = this->selection_start_index;
		end = this->cursor_index;
	}

	auto t = this->get_text();
	this->clear();
	t.erase(utki::next(t.begin(), start), utki::next(t.begin(), end));
	this->set_text(std::move(t));

	return start;
}
