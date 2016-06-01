#include "NinePatchButton.hpp"
#include "../App.hpp"


using namespace morda;


NinePatchButton::NinePatchButton(const stob::Node* chain) :
		Widget(chain),
		NinePatch(chain)
{
	this->imgNormal = morda::App::inst().resMan.load<ResNinePatch>("morda_npt_pushbutton_normal");
	this->imgPressed = morda::App::inst().resMan.load<ResNinePatch>("morda_npt_pushbutton_pressed");
	
	//initialize nine-patch
	this->NinePatchButton::onPressedChanged();
}

void NinePatchButton::onPressedChanged(){
	this->PushButton::onPressedChanged();
	this->setNinePatch(this->isPressed() ? this->imgPressed : this->imgNormal);
}
