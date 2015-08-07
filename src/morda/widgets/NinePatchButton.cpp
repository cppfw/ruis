#include "NinePatchButton.hpp"
#include "../App.hpp"


using namespace morda;


NinePatchButton::NinePatchButton(const stob::Node* chain) :
		Widget(chain),
		NinePatch(chain)
{
	this->imgNormal = morda::App::Inst().resMan.Load<ResNinePatch>("morda_npt_pushbutton");
	this->imgPressed = morda::App::Inst().resMan.Load<ResNinePatch>("morda_npt_pushbutton_pressed");
	
	//initialize nine-patch
	this->NinePatchButton::OnPressedChanged();
}

void NinePatchButton::OnPressedChanged(){
	this->SetNinePatch(this->isPressed() ? this->imgPressed : this->imgNormal);
}
