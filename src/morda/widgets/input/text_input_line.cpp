/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

namespace{
const uint32_t cursorBlinkPeriod_c = 500; // milliseconds

const real cursorWidth_c = real(1.0);
}

text_input_line::text_input_line(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		single_line_text_widget(this->context, desc),
		character_input_widget(this->context),
		color_widget(this->context, desc)
{
	this->set_clip_enabled(true);
}

void text_input_line::render(const morda::matrix4& matrix) const{
	// render selection
	if(this->cursorIndex != this->selectionStartIndex){
		morda::matrix4 matr(matrix);
		matr.translate(
				this->selectionStartIndex < this->cursorIndex ? this->selectionStartPos : this->cursorPos,
				0
			);
		matr.scale(vector2(std::abs(this->cursorPos - this->selectionStartPos), this->rect().d.y()));

		auto& r = *this->context->renderer;
		r.shader->color_pos->render(matr, *r.pos_quad_01_vao, 0xff804040);
	}
	
	{
		morda::matrix4 matr(matrix);
		
		using std::round;
		
		const auto& font = this->get_font().get();

		matr.translate(-this->get_bounding_box().p.x() + this->xOffset, round((font.get_height() + font.get_ascender() - font.get_descender()) / 2));
		
		ASSERT(this->firstVisibleCharIndex <= this->get_text().size())
		font.render(
				matr,
				morda::color_to_vec4f(this->get_current_color()),
				this->get_text().substr(this->firstVisibleCharIndex, this->get_text().size() - this->firstVisibleCharIndex)
			);
	}
	
	if(this->is_focused() && this->cursorBlinkVisible){
		morda::matrix4 matr(matrix);
		matr.translate(this->cursorPos, 0);
		matr.scale(vector2(cursorWidth_c * this->context->units.dots_per_dp, this->rect().d.y()));

		auto& r = *this->context->renderer;
		r.shader->color_pos->render(matr, *r.pos_quad_01_vao, this->get_current_color());
	}
}

bool text_input_line::on_mouse_button(const mouse_button_event& e){
	if(e.button != mouse_button::left){
		return false;
	}

	this->leftMouseButtonDown = e.is_down;
	
	if(e.is_down){
		this->set_cursor_index(this->posToIndex(e.pos.x()));
	}
	
	return true;
}

bool text_input_line::on_mouse_move(const mouse_move_event& e){
	if(!this->leftMouseButtonDown){
		return false;
	}
	
	this->set_cursor_index(this->posToIndex(e.pos.x()), true);
	return true;
}

vector2 text_input_line::measure(const morda::vector2& quotum)const noexcept{
	vector2 ret;
	
	if(quotum.x() < 0){
		ret.x() = this->single_line_text_widget::measure(vector2(-1)).x() + cursorWidth_c * this->context->units.dots_per_dp;
	}else{
		ret.x() = quotum.x();
	}
	
	if(quotum.y() < 0){
		ret.y() = this->get_font().get().get_height();
	}else{
		ret.y() = quotum.y();
	}
	
	return ret;
}

void text_input_line::set_cursor_index(size_t index, bool selection){
	this->cursorIndex = index;
	
	using std::min;
	this->cursorIndex = min(this->cursorIndex, this->get_text().size()); // clamp top
	
	if(!selection){
		this->selectionStartIndex = this->cursorIndex;
	}
	
	utki::scope_exit scopeExit([this](){
		this->selectionStartPos = this->indexToPos(this->selectionStartIndex);
		
		if(!this->is_focused()){
			this->focus();
		}
		this->startCursorBlinking();
	});
	
//	TRACE(<< "selectionStartIndex = " << this->selectionStartIndex << std::endl)
	
	if(this->cursorIndex <= this->firstVisibleCharIndex){
		this->firstVisibleCharIndex = this->cursorIndex;
		this->xOffset = 0;
		this->cursorPos = 0;
		return;
	}
	
	const auto& font = this->get_font().get();

	ASSERT(this->firstVisibleCharIndex <= this->get_text().size())
	ASSERT(this->cursorIndex > this->firstVisibleCharIndex)
	this->cursorPos = font.get_advance(
			std::u32string(this->get_text(), this->firstVisibleCharIndex, this->cursorIndex - this->firstVisibleCharIndex)
		) + this->xOffset;
	
	ASSERT(this->cursorPos >= 0)
	
	if(this->cursorPos > this->rect().d.x() - cursorWidth_c * this->context->units.dots_per_dp){
		this->cursorPos = this->rect().d.x() - cursorWidth_c * this->context->units.dots_per_dp;
		
		this->xOffset = this->cursorPos; // start from rightmost cursor position
		this->firstVisibleCharIndex = this->cursorIndex;
		
		// calculate advance backwards
		for(auto i = this->get_text().rbegin() + (this->get_text().size() - this->cursorIndex);
				this->xOffset > 0;
				++i
			)
		{
			ASSERT(i != this->get_text().rend())
			this->xOffset -= font.get_advance(*i);
			ASSERT(this->firstVisibleCharIndex > 0)
			--this->firstVisibleCharIndex;
		}
	}
}

real text_input_line::indexToPos(size_t index){
	ASSERT(this->firstVisibleCharIndex <= this->get_text().size())
	
	if(index <= this->firstVisibleCharIndex){
		return 0;
	}
	
	using std::min;
	index = min(index, this->get_text().size()); // clamp top
	
	real ret = this->xOffset;
	
	for(auto i = this->get_text().begin() + this->firstVisibleCharIndex;
			i != this->get_text().end() && index != this->firstVisibleCharIndex;
			++i, --index
		)
	{
		ret += this->get_font().get().get_advance(*i);
		if(ret >= this->rect().d.x()){
			ret = this->rect().d.x();
			break;
		}
	}
	
	return ret;
}

size_t text_input_line::posToIndex(real pos){
	size_t index = this->firstVisibleCharIndex;
	real p = this->xOffset;
	
	for(auto i = this->get_text().begin() + this->firstVisibleCharIndex; i != this->get_text().end(); ++i){
		real w = this->get_font().get().get_advance(*i);
		
		if(pos < p + w){
			if(pos < p + w / 2){
				break;
			}
			p += w;
			++index;
			break;
		}
		
		p += w;
		++index;
	}
	
	return index;
}


void text_input_line::update(uint32_t dt){
	this->cursorBlinkVisible = !this->cursorBlinkVisible;
}

void text_input_line::on_focus_change(){
	if(this->is_focused()){
		this->ctrlPressed = false;
		this->shiftPressed = false;
		this->startCursorBlinking();
	}else{
		this->context->updater->stop(*this);
	}
}

void text_input_line::on_resize(){
//	TRACE(<< "text_input_line::on_resize(): size = " << this->rect().d << std::endl)
	this->selectionStartPos = this->indexToPos(this->selectionStartIndex);
}


void text_input_line::startCursorBlinking(){
	this->context->updater->stop(*this);
	this->cursorBlinkVisible = true;
	this->context->updater->start(
			utki::make_shared_from(*static_cast<updateable*>(this)),
			cursorBlinkPeriod_c
		);
}

bool text_input_line::on_key(const morda::key_event& e){
	switch(e.key){
		case morda::key::left_control:
		case morda::key::right_control:
			this->ctrlPressed = e.is_down;
			break;
		case morda::key::left_shift:
		case morda::key::right_shift:
			this->shiftPressed = e.is_down;
			break;
		default:
			break;
	}
	return false;
}

void text_input_line::on_character_input(const character_input_event& e){
	switch(e.key){
		case morda::key::enter:
			break;
		case morda::key::right:
			if(this->cursorIndex != this->get_text().size()){
				size_t newIndex;
				if(this->ctrlPressed){
					bool spaceSkipped = false;
					newIndex = this->cursorIndex;
					for(auto i = this->get_text().begin() + this->cursorIndex; i != this->get_text().end(); ++i, ++newIndex){
						if(*i == uint32_t(' ')){
							if(spaceSkipped){
								break;
							}
						}else{
							spaceSkipped = true;
						}
					}

				}else{
					newIndex = this->cursorIndex + 1;
				}
				this->set_cursor_index(newIndex, this->shiftPressed);
			}
			break;
		case morda::key::left:
			if(this->cursorIndex != 0){
				size_t newIndex;
				if(this->ctrlPressed){
					bool spaceSkipped = false;
					newIndex = this->cursorIndex;
					for(auto i = this->get_text().rbegin() + (this->get_text().size() - this->cursorIndex);
							i != this->get_text().rend();
							++i, --newIndex
						)
					{
						if(*i == uint32_t(' ')){
							if(spaceSkipped){
								break;
							}
						}else{
							spaceSkipped = true;
						}
					}
				}else{
					newIndex = this->cursorIndex - 1;
				}
				this->set_cursor_index(newIndex, this->shiftPressed);
			}
			break;
		case morda::key::end:
			this->set_cursor_index(this->get_text().size(), this->shiftPressed);
			break;
		case morda::key::home:
			this->set_cursor_index(0, this->shiftPressed);
			break;
		case morda::key::backspace:
			if(this->thereIsSelection()){
				this->set_cursor_index(this->deleteSelection());
			}else{
				if(this->cursorIndex != 0){
					auto t = this->get_text();
					this->clear();
					t.erase(t.begin() + (this->cursorIndex - 1));
					this->set_text(std::move(t));
					this->set_cursor_index(this->cursorIndex - 1);
				}
			}
			break;
		case morda::key::deletion:
			if(this->thereIsSelection()){
				this->set_cursor_index(this->deleteSelection());
			}else{
				if(this->cursorIndex < this->get_text().size()){
					auto t = this->get_text();
					this->clear();
					t.erase(t.begin() + this->cursorIndex);
					this->set_text(std::move(t));
				}
			}
			this->startCursorBlinking();
			break;
		case morda::key::escape:
			// do nothing
			break;
		case morda::key::a:
			if(this->ctrlPressed){
				this->selectionStartIndex = 0;
				this->set_cursor_index(this->get_text().size(), true);
				break;
			}
			// fall through
		default:
			if(!e.unicode.empty()){
				if(this->thereIsSelection()){
					this->cursorIndex = this->deleteSelection();
				}
				
				auto t = this->get_text();
				this->clear();
				t.insert(t.begin() + this->cursorIndex, e.unicode.begin(), e.unicode.end());
				this->set_text(std::move(t));
				
				this->set_cursor_index(this->cursorIndex + e.unicode.size());
			}
			
			break;
	}
	
}

size_t text_input_line::deleteSelection(){
	ASSERT(this->cursorIndex != this->selectionStartIndex)
	
	size_t start, end;
	if(this->cursorIndex < this->selectionStartIndex){
		start = this->cursorIndex;
		end = this->selectionStartIndex;
	}else{
		start = this->selectionStartIndex;
		end = this->cursorIndex;
	}
	
	auto t = this->get_text();
	this->clear();
	t.erase(t.begin() + start, t.begin() + end);
	this->set_text(std::move(t));
	
	return start;
}
