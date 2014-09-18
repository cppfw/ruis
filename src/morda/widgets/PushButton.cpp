#include "PushButton.hpp"
#include "../App.hpp"


using namespace morda;


PushButton::PushButton(const stob::Node* desc) :
		Widget(desc),
		NinePatch(desc)
{
	this->imgNormal = morda::App::Inst().resMan.Load<ResNinePatch>("morda_npt_pushbutton");
	this->imgPressed = morda::App::Inst().resMan.Load<ResNinePatch>("morda_npt_pushbutton_pressed");
	
	//initialize nine-patch
	this->PushButton::OnPressedChanged();
	
	if(!desc){
		return;
	}
	
	
}

void PushButton::OnPressedChanged(){
	this->SetNinePatch(this->IsPressed() ? this->imgPressed : this->imgNormal);
}
