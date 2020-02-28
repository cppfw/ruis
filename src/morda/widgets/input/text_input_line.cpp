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
const std::uint32_t cursorBlinkPeriod_c = 500; // milliseconds

const real cursorWidth_c = real(1.0);
}

text_input_line::text_input_line(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		single_line_text_widget(this->context, desc),
		character_input_widget(this->context)
{
	this->set_clip(true);
}

void text_input_line::render(const morda::matrix4& matrix) const{
	// render selection
	if(this->cursorIndex != this->selectionStartIndex){
		morda::matrix4 matr(matrix);
		matr.translate(
				this->selectionStartIndex < this->cursorIndex ? this->selectionStartPos : this->cursorPos,
				0
			);
		matr.scale(Vec2r(std::abs(this->cursorPos - this->selectionStartPos), this->rect().d.y));

		auto& r = *this->context->renderer;
		r.shader->color_pos->render(matr, *r.pos_quad_01_vao, 0xff804040);
	}
	
	{
		morda::matrix4 matr(matrix);
		
		using std::round;
		
		matr.translate(-this->get_bounding_box().p.x + this->xOffset, round((this->font().get_height() + this->font().get_ascender() - this->font().get_descender()) / 2));
		
		ASSERT(this->firstVisibleCharIndex <= this->get_text().size())
		this->font().render(
				matr,
				morda::colorToVec4f(this->get_color()),
				this->get_text().substr(this->firstVisibleCharIndex, this->get_text().size() - this->firstVisibleCharIndex)
			);
	}
	
	if(this->is_focused() && this->cursorBlinkVisible){
		morda::matrix4 matr(matrix);
		matr.translate(this->cursorPos, 0);
		matr.scale(Vec2r(cursorWidth_c * this->context->units.dots_per_dp, this->rect().d.y));

		auto& r = *this->context->renderer;
		r.shader->color_pos->render(matr, *r.pos_quad_01_vao, this->get_color());
	}
}

bool text_input_line::on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerId){
	if(button != mouse_button::left){
		return false;
	}

	this->leftMouseButtonDown = isDown;
	
	if(isDown){
		this->set_cursor_index(this->posToIndex(pos.x));
	}
	
	return true;
}

bool text_input_line::on_mouse_move(const morda::Vec2r& pos, unsigned pointerId){
	if(!this->leftMouseButtonDown){
		return false;
	}
	
	this->set_cursor_index(this->posToIndex(pos.x), true);
	return true;
}


Vec2r text_input_line::measure(const morda::Vec2r& quotum)const noexcept{
	Vec2r ret;
	
	if(quotum.x < 0){
		ret.x = this->single_line_text_widget::measure(Vec2r(-1)).x + cursorWidth_c * this->context->units.dots_per_dp;
	}else{
		ret.x = quotum.x;
	}
	
	if(quotum.y < 0){
		ret.y = this->font().get_height();
	}else{
		ret.y = quotum.y;
	}
	
	return ret;
}

void text_input_line::set_cursor_index(size_t index, bool selection){
	this->cursorIndex = index;
	
	utki::clampTop(this->cursorIndex, this->get_text().size());
	
	if(!selection){
		this->selectionStartIndex = this->cursorIndex;
	}
	
	utki::ScopeExit scopeExit([this](){
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
	
	ASSERT(this->firstVisibleCharIndex <= this->get_text().size())
	ASSERT(this->cursorIndex > this->firstVisibleCharIndex)
	this->cursorPos = this->font().get_advance(
			std::u32string(this->get_text(), this->firstVisibleCharIndex, this->cursorIndex - this->firstVisibleCharIndex)
		) + this->xOffset;
	
	ASSERT(this->cursorPos >= 0)
	
	if(this->cursorPos > this->rect().d.x - cursorWidth_c * this->context->units.dots_per_dp){
		this->cursorPos = this->rect().d.x - cursorWidth_c * this->context->units.dots_per_dp;
		
		this->xOffset = this->cursorPos; // start from rightmost cursor position
		this->firstVisibleCharIndex = this->cursorIndex;
		
		// calculate advance backwards
		for(auto i = this->get_text().rbegin() + (this->get_text().size() - this->cursorIndex);
				this->xOffset > 0;
				++i
			)
		{
			ASSERT(i != this->get_text().rend())
			this->xOffset -= this->font().get_advance(*i);
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
	
	utki::clampTop(index, this->get_text().size());
	
	real ret = this->xOffset;
	
	for(auto i = this->get_text().begin() + this->firstVisibleCharIndex;
			i != this->get_text().end() && index != this->firstVisibleCharIndex;
			++i, --index
		)
	{
		ret += this->font().get_advance(*i);
		if(ret >= this->rect().d.x){
			ret = this->rect().d.x;
			break;
		}
	}
	
	return ret;
}

size_t text_input_line::posToIndex(real pos){
	size_t index = this->firstVisibleCharIndex;
	real p = this->xOffset;
	
	for(auto i = this->get_text().begin() + this->firstVisibleCharIndex; i != this->get_text().end(); ++i){
		real w = this->font().get_advance(*i);
		
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


void text_input_line::update(std::uint32_t dt){
	this->cursorBlinkVisible = !this->cursorBlinkVisible;
}

void text_input_line::on_focus_changed(){
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
			std::dynamic_pointer_cast<updateable>(this->shared_from_this()),
			cursorBlinkPeriod_c
		);
}

bool text_input_line::on_key(bool isDown, key keyCode){
	switch(keyCode){
		case key::left_control:
		case key::right_control:
			this->ctrlPressed = isDown;
			break;
		case key::left_shift:
		case key::right_shift:
			this->shiftPressed = isDown;
			break;
		default:
			break;
	}
	return false;
}

void text_input_line::on_character_input(const std::u32string& unicode, key keycode){
	switch(keycode){
		case key::enter:
			break;
		case key::right:
			if(this->cursorIndex != this->get_text().size()){
				size_t newIndex;
				if(this->ctrlPressed){
					bool spaceSkipped = false;
					newIndex = this->cursorIndex;
					for(auto i = this->get_text().begin() + this->cursorIndex; i != this->get_text().end(); ++i, ++newIndex){
						if(*i == std::uint32_t(' ')){
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
		case key::left:
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
						if(*i == std::uint32_t(' ')){
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
		case key::end:
			this->set_cursor_index(this->get_text().size(), this->shiftPressed);
			break;
		case key::home:
			this->set_cursor_index(0, this->shiftPressed);
			break;
		case key::backspace:
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
		case key::deletion:
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
		case key::escape:
			// do nothing
			break;
		case key::a:
			if(this->ctrlPressed){
				this->selectionStartIndex = 0;
				this->set_cursor_index(this->get_text().size(), true);
				break;
			}
			// fall through
		default:
			if(unicode.size() != 0){
				if(this->thereIsSelection()){
					this->cursorIndex = this->deleteSelection();
				}
				
				auto t = this->get_text();
				this->clear();
				t.insert(t.begin() + this->cursorIndex, unicode.begin(), unicode.end());
				this->set_text(std::move(t));
				
				this->set_cursor_index(this->cursorIndex + unicode.size());
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
