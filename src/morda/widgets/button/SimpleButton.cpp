#include "SimpleButton.hpp"
#include "../../App.hpp"
#include "../../util/util.hpp"


using namespace morda;


SimpleButton::SimpleButton(const stob::Node* chain) :
		Widget(chain),
		NinePatch(chain)
{
	{
		auto ch = getProperty(chain, "look");
		
		if(auto n = getProperty(ch, "imageNormal")){
			this->imgNormal = morda::App::inst().resMan.load<ResNinePatch>(n->value());
		}else{
			this->imgNormal = morda::App::inst().resMan.load<ResNinePatch>("morda_npt_pushbutton_normal");
		}

		if(auto n = getProperty(ch, "imagePressed")){
			this->imgPressed = morda::App::inst().resMan.load<ResNinePatch>(n->value());
		}else{
			this->imgPressed = morda::App::inst().resMan.load<ResNinePatch>("morda_npt_pushbutton_pressed");
		}
	}
	
	//initialize nine-patch
	this->SimpleButton::onPressedChanged();
}

void SimpleButton::onPressedChanged(){
	this->PushButton::onPressedChanged();
	this->setNinePatch(this->isPressed() ? this->imgPressed : this->imgNormal);
}
