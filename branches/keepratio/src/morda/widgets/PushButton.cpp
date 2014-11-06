#include "PushButton.hpp"
#include "../App.hpp"


using namespace morda;


PushButton::PushButton(const stob::Node* chain) :
		Widget(chain),
		NinePatch(chain)
{
	this->imgNormal = morda::App::Inst().resMan.Load<ResNinePatch>("morda_npt_pushbutton");
	this->imgPressed = morda::App::Inst().resMan.Load<ResNinePatch>("morda_npt_pushbutton_pressed");
	
	//initialize nine-patch
	this->PushButton::OnPressedChanged();
}

void PushButton::OnPressedChanged(){
	this->SetNinePatch(this->IsPressed() ? this->imgPressed : this->imgNormal);
}
