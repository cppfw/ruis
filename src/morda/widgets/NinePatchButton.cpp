#include "NinePatchButton.hpp"
#include "../App.hpp"
#include "../util/util.hpp"


using namespace morda;


NinePatchButton::NinePatchButton(const stob::Node* chain) :
		Widget(chain),
		NinePatch(chain)
{
	if(auto n = getProperty(chain, "imageNormal")){
		this->imgNormal = morda::App::inst().resMan.load<ResNinePatch>(n->value());
	}else{
		this->imgNormal = morda::App::inst().resMan.load<ResNinePatch>("morda_npt_pushbutton_normal");
	}
	
	if(auto n = getProperty(chain, "imagePressed")){
		this->imgPressed = morda::App::inst().resMan.load<ResNinePatch>(n->value());
	}else{
		this->imgPressed = morda::App::inst().resMan.load<ResNinePatch>("morda_npt_pushbutton_pressed");
	}
	
	//initialize nine-patch
	this->NinePatchButton::onPressedChanged();
}

void NinePatchButton::onPressedChanged(){
	this->PushButton::onPressedChanged();
	this->setNinePatch(this->isPressed() ? this->imgPressed : this->imgNormal);
}
