#include "TextInputLine.hpp"

#include "../../Morda.hpp"
#include "../../util/util.hpp"



#if M_OS == M_OS_WINDOWS
#	ifdef DELETE
#		undef DELETE
#	endif
#endif



using namespace morda;



namespace{
const std::uint32_t cursorBlinkPeriod_c = 500; //milliseconds

const real cursorWidth_c = real(1.0);
}



TextInputLine::TextInputLine(const stob::Node* chain) :
		Widget(chain),
		SingleLineTextWidget(chain)
{
	this->setClip(true);
}



void TextInputLine::render(const morda::Matr4r& matrix) const{
	//render selection
	if(this->cursorIndex != this->selectionStartIndex){
		morda::Matr4r matr(matrix);
		matr.translate(
				this->selectionStartIndex < this->cursorIndex ? this->selectionStartPos : this->cursorPos,
				0
			);
		matr.scale(Vec2r(std::abs(this->cursorPos - this->selectionStartPos), this->rect().d.y));

		auto& r = morda::inst().renderer();
		r.shader->colorPos->render(matr, *r.posQuad01VAO, 0xff804040);
	}
	
	{
		morda::Matr4r matr(matrix);
		
		using std::round;
		
		matr.translate(-this->textBoundingBox().p.x + this->xOffset, round((this->font().height() + this->font().ascender() - this->font().descender()) / 2));
		
		ASSERT(this->firstVisibleCharIndex <= this->getText().size())
		this->font().renderString(
				matr,
				morda::colorToVec4f(this->color()),
				this->getText().substr(this->firstVisibleCharIndex, this->getText().size() - this->firstVisibleCharIndex)
			);
	}
	
	if(this->isFocused() && this->cursorBlinkVisible){
		morda::Matr4r matr(matrix);
		matr.translate(this->cursorPos, 0);
		matr.scale(Vec2r(cursorWidth_c * morda::inst().units.dotsPerPt(), this->rect().d.y));

		auto& r = morda::inst().renderer();
		r.shader->colorPos->render(matr, *r.posQuad01VAO, this->color());
	}
}

bool TextInputLine::onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId){
	if(button != MouseButton_e::LEFT){
		return false;
	}

	this->leftMouseButtonDown = isDown;
	
	if(isDown){
		this->setCursorIndex(this->posToIndex(pos.x));
	}
	
	return true;
}

bool TextInputLine::onMouseMove(const morda::Vec2r& pos, unsigned pointerId){
	if(!this->leftMouseButtonDown){
		return false;
	}
	
	this->setCursorIndex(this->posToIndex(pos.x), true);
	return true;
}


Vec2r TextInputLine::measure(const morda::Vec2r& quotum)const noexcept{
	Vec2r ret;
	
	if(quotum.x < 0){
		ret.x = this->SingleLineTextWidget::measure(Vec2r(-1)).x + cursorWidth_c * morda::inst().units.dotsPerPt();
	}else{
		ret.x = quotum.x;
	}
	
	if(quotum.y < 0){
		ret.y = this->font().height();
	}else{
		ret.y = quotum.y;
	}
	
	return ret;
}

void TextInputLine::setCursorIndex(size_t index, bool selection){
	this->cursorIndex = index;
	
	utki::clampTop(this->cursorIndex, this->getText().size());
	
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
	
	ASSERT(this->firstVisibleCharIndex <= this->getText().size())
	ASSERT(this->cursorIndex > this->firstVisibleCharIndex)
	this->cursorPos = this->font().stringAdvance(
			std::u32string(this->getText(), this->firstVisibleCharIndex, this->cursorIndex - this->firstVisibleCharIndex)
		) + this->xOffset;
	
	ASSERT(this->cursorPos >= 0)
	
	if(this->cursorPos > this->rect().d.x - cursorWidth_c * morda::inst().units.dotsPerPt()){
		this->cursorPos = this->rect().d.x - cursorWidth_c * morda::inst().units.dotsPerPt();
		
		this->xOffset = this->cursorPos;//start from rightmost cursor position
		this->firstVisibleCharIndex = this->cursorIndex;
		
		//calculate advance backwards
		for(auto i = this->getText().rbegin() + (this->getText().size() - this->cursorIndex);
				this->xOffset > 0;
				++i
			)
		{
			ASSERT(i != this->getText().rend())
			this->xOffset -= this->font().charAdvance(*i);
			ASSERT(this->firstVisibleCharIndex > 0)
			--this->firstVisibleCharIndex;
		}
	}
}



real TextInputLine::indexToPos(size_t index){
	ASSERT(this->firstVisibleCharIndex <= this->getText().size())
	
	if(index <= this->firstVisibleCharIndex){
		return 0;
	}
	
	utki::clampTop(index, this->getText().size());
	
	real ret = this->xOffset;
	
	for(auto i = this->getText().begin() + this->firstVisibleCharIndex;
			i != this->getText().end() && index != this->firstVisibleCharIndex;
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


size_t TextInputLine::posToIndex(real pos){
	size_t index = this->firstVisibleCharIndex;
	real p = this->xOffset;
	
	for(auto i = this->getText().begin() + this->firstVisibleCharIndex; i != this->getText().end(); ++i){
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


void TextInputLine::update(std::uint32_t dt){
	this->cursorBlinkVisible = !this->cursorBlinkVisible;
}

void TextInputLine::onFocusChanged(){
	if(this->isFocused()){
		this->ctrlPressed = false;
		this->shiftPressed = false;
		this->startCursorBlinking();
	}else{
		this->stopUpdating();
	}
}

void TextInputLine::onResize(){
	TRACE(<< "TextInputLine::onResize(): size = " << this->rect().d << std::endl)
	this->selectionStartPos = this->indexToPos(this->selectionStartIndex);
}


void TextInputLine::startCursorBlinking(){
	this->stopUpdating();
	this->cursorBlinkVisible = true;
	this->startUpdating(cursorBlinkPeriod_c);
}

bool TextInputLine::onKey(bool isDown, Key_e keyCode){
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

void TextInputLine::onCharacterInput(const std::u32string& unicode, Key_e key){
	switch(key){
		case Key_e::ENTER:
			break;
		case Key_e::RIGHT:
			if(this->cursorIndex != this->getText().size()){
				size_t newIndex;
				if(this->ctrlPressed){
					bool spaceSkipped = false;
					newIndex = this->cursorIndex;
					for(auto i = this->getText().begin() + this->cursorIndex; i != this->getText().end(); ++i, ++newIndex){
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
					for(auto i = this->getText().rbegin() + (this->getText().size() - this->cursorIndex);
							i != this->getText().rend();
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
			this->setCursorIndex(this->getText().size(), this->shiftPressed);
			break;
		case Key_e::HOME:
			this->setCursorIndex(0, this->shiftPressed);
			break;
		case Key_e::BACKSPACE:
			if(this->thereIsSelection()){
				this->setCursorIndex(this->deleteSelection());
			}else{
				if(this->cursorIndex != 0){
					auto t = this->getText();
					this->clear();
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
				if(this->cursorIndex < this->getText().size()){
					auto t = this->getText();
					this->clear();
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
				this->setCursorIndex(this->getText().size(), true);
				break;
			}
			//fall through
		default:
			if(unicode.size() != 0){
				if(this->thereIsSelection()){
					this->cursorIndex = this->deleteSelection();
				}
				
				auto t = this->getText();
				this->clear();
				t.insert(t.begin() + this->cursorIndex, unicode.begin(), unicode.end());
				this->setText(std::move(t));
				
				this->setCursorIndex(this->cursorIndex + unicode.size());
			}
			
			break;
		
	}
	
}



size_t TextInputLine::deleteSelection(){
	ASSERT(this->cursorIndex != this->selectionStartIndex)
	
	size_t start, end;
	if(this->cursorIndex < this->selectionStartIndex){
		start = this->cursorIndex;
		end = this->selectionStartIndex;
	}else{
		start = this->selectionStartIndex;
		end = this->cursorIndex;
	}
	
	auto t = this->getText();
	this->clear();
	t.erase(t.begin() + start, t.begin() + end);
	this->setText(std::move(t));
	
	return start;
}
