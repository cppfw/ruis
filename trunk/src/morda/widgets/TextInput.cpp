#include "TextInput.hpp"

#include "../App.hpp"



using namespace morda;



namespace{
std::uint32_t D_CursorBlinkPeriod = 500; //milliseconds
}



TextInput::TextInput(const stob::Node* desc) :
		Widget(desc),
		SingleLineTextWidget(desc)
{
	if(!desc){
		return;
	}
}



void TextInput::Render(const morda::Matr4r& matrix) const{
	this->Font().RenderString(matrix, this->Text());
	
	if(this->IsFocused() && this->cursorBlinkVisible){
		morda::Matr4r matr(matrix);
		matr.Translate(this->cursorPos, 0);
		matr.Scale(Vec2r(2, this->Font().Size()));

		ColorPosShader& s = App::Inst().Shaders().simpleSingleColoring;
		s.Bind();
		s.SetColor(0xffffffff);

		s.SetMatrix(matr);
		s.Render(s.quad01Fan, Shader::EMode::TRIANGLE_FAN);
	}
}

bool TextInput::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
	if(!isDown){
		return false;
	}
	
	if(button != EMouseButton::LEFT){
		return false;
	}
	
	this->SetCursor(pos.x);
	
	return true;
}



void TextInput::SetCursor(real toPos){
	this->cursorPos = 0;
	this->cursorIndex = 0;
	
	for(auto c : this->Text()){
		real w = this->Font().CharAdvance(c);
		
		if(toPos > this->cursorPos){
			this->cursorPos += w;
			++this->cursorIndex;
			if(toPos < this->cursorPos){
				break;
			}
		}		
	}
	
//	TRACE(<< "this->cursorPos = " << this->cursorPos << std::endl)
	
	this->cursorBlinkVisible = true;
	
	if(!this->IsFocused()){
		this->Focus();
	}
}



void TextInput::Update(std::uint32_t dt){
	this->cursorBlinkVisible = !this->cursorBlinkVisible;
}

void TextInput::OnFocusedChanged(){
	if(this->IsFocused()){
		this->StartCursorBlinking();
	}else{
		this->StopUpdating();
	}
}

void TextInput::StartCursorBlinking(){
	this->StopUpdating();
	this->StartUpdating(D_CursorBlinkPeriod);
}



void TextInput::OnCharacterInput(ting::Buffer<const std::uint32_t> unicode, EKey key){
	switch(key){
		case EKey::RIGHT:
			++this->cursorIndex;
			ting::util::ClampTop(this->cursorIndex, this->Text().size());
			this->UpdateCursorPosBasedOnIndex();
			this->cursorBlinkVisible = true;
			break;
		case EKey::LEFT:
			if(this->cursorIndex != 0){
				--this->cursorIndex;
				this->UpdateCursorPosBasedOnIndex();
				this->cursorBlinkVisible = true;
			}
			break;
		default:
			break;
		
	}
	
}

void TextInput::UpdateCursorPosBasedOnIndex(){
	this->cursorPos = this->Font().StringAdvance(ting::Buffer<const std::uint32_t>(&*this->Text().begin(), this->cursorIndex));
}
