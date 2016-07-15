#include "TextInput.hpp"

#include "../App.hpp"



#if M_OS == M_OS_WINDOWS
#	ifdef DELETE
#		undef DELETE
#	endif
#endif



using namespace morda;



namespace{
std::uint32_t D_CursorBlinkPeriod = 500; //milliseconds

real D_CursorWidth = real(2.0);
}



TextInput::TextInput(const stob::Node* chain) :
		Widget(chain),
		SingleLineTextWidget(chain)
{
	this->setClip(true);
}



void TextInput::render(const morda::Matr4r& matrix) const{
	//render selection
	if(this->cursorIndex != this->selectionStartIndex){
		morda::Matr4r matr(matrix);
		matr.translate(
				this->selectionStartIndex < this->cursorIndex ? this->selectionStartPos : this->cursorPos,
				0
			);
		matr.scale(Vec2r(std::abs(this->cursorPos - this->selectionStartPos), this->rect().d.y));

		ColorPosShader& s = App::inst().shaders.colorPosShader;

		s.setColor(0xff804040);

		s.setMatrix(matr);
		s.render(utki::wrapBuf(s.quad01Fan));
	}
	
	{
		morda::Matr4r matr(matrix);
		matr.translate(-this->textBoundingBox().p.x + this->xOffset, -this->font().boundingBox().p.y);
		
		PosTexShader& s = [this]() -> PosTexShader&{
			if(this->color() == 0xffffffff){//if white
				return morda::App::inst().shaders.posTexShader;
			}else{
				ColorPosTexShader& s = morda::App::inst().shaders.colorPosTexShader;

				s.setColor(this->color());
				return s;
			}
		}();
		
		ASSERT(this->firstVisibleCharIndex <= this->text().size())
		this->font().renderString(
				s,
				matr,
				std::u32string(this->text(), this->firstVisibleCharIndex, this->text().size() - this->firstVisibleCharIndex)
			);
	}
	
	if(this->isFocused() && this->cursorBlinkVisible){
		morda::Matr4r matr(matrix);
		matr.translate(this->cursorPos, 0);
		matr.scale(Vec2r(D_CursorWidth, this->rect().d.y));

		ColorPosShader& s = App::inst().shaders.colorPosShader;

		s.setColor(this->color());

		s.setMatrix(matr);
		s.render(utki::wrapBuf(s.quad01Fan));
	}
}

bool TextInput::onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
	if(button != EMouseButton::LEFT){
		return false;
	}

	this->leftMouseButtonDown = isDown;
	
	if(isDown){
		this->setCursorIndex(this->posToIndex(pos.x));
	}
	
	return true;
}

bool TextInput::onMouseMove(const morda::Vec2r& pos, unsigned pointerId){
	if(!this->leftMouseButtonDown){
		return false;
	}
	
	this->setCursorIndex(this->posToIndex(pos.x), true);
	return true;
}


Vec2r TextInput::measure(const morda::Vec2r& quotum)const noexcept{
	Vec2r ret;
	
	if(quotum.x < 0){
		ret.x = this->SingleLineTextWidget::measure(Vec2r(-1)).x + D_CursorWidth;
	}else{
		ret.x = quotum.x;
	}
	
	if(quotum.y < 0){
		ret.y = this->font().boundingBox().d.y - this->font().boundingBox().p.y;
	}else{
		ret.y = quotum.y;
	}
	
	return ret;
}

void TextInput::setCursorIndex(size_t index, bool selection){
	this->cursorIndex = index;
	
	utki::clampTop(this->cursorIndex, this->text().size());
	
	if(!selection){
		this->selectionStartIndex = this->cursorIndex;
	}
	
	utki::ScopeExit scopeExit([this](){
		this->selectionStartPos = this->indexToPos(this->selectionStartIndex);
		
		if(!this->isFocused()){
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
	
	ASSERT(this->firstVisibleCharIndex <= this->text().size())
	ASSERT(this->cursorIndex > this->firstVisibleCharIndex)
	this->cursorPos = this->font().stringAdvance(
			std::u32string(this->text(), this->firstVisibleCharIndex, this->cursorIndex - this->firstVisibleCharIndex)
		) + this->xOffset;
	
	ASSERT(this->cursorPos >= 0)
	
	if(this->cursorPos > this->rect().d.x - D_CursorWidth){
		this->cursorPos = this->rect().d.x - D_CursorWidth;
		
		this->xOffset = this->cursorPos;//start from rightmost cursor position
		this->firstVisibleCharIndex = this->cursorIndex;
		
		//calculate advance backwards
		for(auto i = this->text().rbegin() + (this->text().size() - this->cursorIndex);
				this->xOffset > 0;
				++i
			)
		{
			ASSERT(i != this->text().rend())
			this->xOffset -= this->font().charAdvance(*i);
			ASSERT(this->firstVisibleCharIndex > 0)
			--this->firstVisibleCharIndex;
		}
	}
}



real TextInput::indexToPos(size_t index){
	ASSERT(this->firstVisibleCharIndex <= this->text().size())
	
	if(index <= this->firstVisibleCharIndex){
		return 0;
	}
	
	utki::clampTop(index, this->text().size());
	
	real ret = this->xOffset;
	
	for(auto i = this->text().begin() + this->firstVisibleCharIndex;
			i != this->text().end() && index != this->firstVisibleCharIndex;
			++i, --index
		)
	{
		ret += this->font().charAdvance(*i);
		if(ret >= this->rect().d.x){
			ret = this->rect().d.x;
			break;
		}
	}
	
	return ret;
}


size_t TextInput::posToIndex(real pos){
	size_t index = this->firstVisibleCharIndex;
	real p = this->xOffset;
	
	for(auto i = this->text().begin() + this->firstVisibleCharIndex; i != this->text().end(); ++i){
		real w = this->font().charAdvance(*i);
		
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


void TextInput::update(std::uint32_t dt){
	this->cursorBlinkVisible = !this->cursorBlinkVisible;
}

void TextInput::onFocusChanged(){
	if(this->isFocused()){
		this->ctrlPressed = false;
		this->shiftPressed = false;
		this->startCursorBlinking();
	}else{
		this->stopUpdating();
	}
}

void TextInput::onResize(){
	this->selectionStartPos = this->indexToPos(this->selectionStartIndex);
}


void TextInput::startCursorBlinking(){
	this->stopUpdating();
	this->cursorBlinkVisible = true;
	this->startUpdating(D_CursorBlinkPeriod);
}

bool TextInput::onKey(bool isDown, Key_e keyCode){
	switch(keyCode){
		case Key_e::LEFT_CONTROL:
		case Key_e::RIGHT_CONTROL:
			this->ctrlPressed = isDown;
			break;
		case Key_e::LEFT_SHIFT:
		case Key_e::RIGHT_SHIFT:
			this->shiftPressed = isDown;
			break;
		default:
			break;
	}
	return false;
}

void TextInput::onCharacterInput(const utki::Buf<std::uint32_t> unicode, Key_e key){
	switch(key){
		case Key_e::ENTER:
			break;
		case Key_e::RIGHT:
			if(this->cursorIndex != this->text().size()){
				size_t newIndex;
				if(this->ctrlPressed){
					bool spaceSkipped = false;
					newIndex = this->cursorIndex;
					for(auto i = this->text().begin() + this->cursorIndex; i != this->text().end(); ++i, ++newIndex){
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
				this->setCursorIndex(newIndex, this->shiftPressed);
			}
			break;
		case Key_e::LEFT:
			if(this->cursorIndex != 0){
				size_t newIndex;
				if(this->ctrlPressed){
					bool spaceSkipped = false;
					newIndex = this->cursorIndex;
					for(auto i = this->text().rbegin() + (this->text().size() - this->cursorIndex);
							i != this->text().rend();
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
				this->setCursorIndex(newIndex, this->shiftPressed);
			}
			break;
		case Key_e::END:
			this->setCursorIndex(this->text().size(), this->shiftPressed);
			break;
		case Key_e::HOME:
			this->setCursorIndex(0, this->shiftPressed);
			break;
		case Key_e::BACKSPACE:
			if(this->thereIsSelection()){
				this->setCursorIndex(this->deleteSelection());
			}else{
				if(this->cursorIndex != 0){
					auto t = this->clear();
					t.erase(t.begin() + (this->cursorIndex - 1));
					this->setText(std::move(t));
					this->setCursorIndex(this->cursorIndex - 1);
				}
			}
			break;
		case Key_e::DELETE:
			if(this->thereIsSelection()){
				this->setCursorIndex(this->deleteSelection());
			}else{
				if(this->cursorIndex < this->text().size()){
					auto t = this->clear();
					t.erase(t.begin() + this->cursorIndex);
					this->setText(std::move(t));
				}
			}
			this->startCursorBlinking();
			break;
		case Key_e::ESCAPE:
			//do nothing
			break;
		case Key_e::A:
			if(this->ctrlPressed){
				this->selectionStartIndex = 0;
				this->setCursorIndex(this->text().size(), true);
				break;
			}
			//fall through
		default:
			if(unicode.size() != 0){
				if(this->thereIsSelection()){
					this->cursorIndex = this->deleteSelection();
				}
				
				auto t = this->clear();
				t.insert(t.begin() + this->cursorIndex, unicode.begin(), unicode.end());
				this->setText(std::move(t));
				
				this->setCursorIndex(this->cursorIndex + unicode.size());
			}
			
			break;
		
	}
	
}



size_t TextInput::deleteSelection(){
	ASSERT(this->cursorIndex != this->selectionStartIndex)
	
	size_t start, end;
	if(this->cursorIndex < this->selectionStartIndex){
		start = this->cursorIndex;
		end = this->selectionStartIndex;
	}else{
		start = this->selectionStartIndex;
		end = this->cursorIndex;
	}
	
	auto t = this->clear();
	t.erase(t.begin() + start, t.begin() + end);
	this->setText(std::move(t));
	
	return start;
}
