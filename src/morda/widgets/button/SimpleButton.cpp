#include "SimpleButton.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"


using namespace morda;


SimpleButton::SimpleButton(const stob::Node* chain) :
		Widget(chain),
		NinePatch(chain)
{
	{
		auto ch = getProperty(chain, "look");
		
		if(auto n = getProperty(ch, "imageNormal")){
			this->imgNormal = morda::Morda::inst().resMan.load<ResNinePatch>(n->value());
		}else{
			this->imgNormal = morda::Morda::inst().resMan.load<ResNinePatch>("morda_npt_button_normal");
		}

		if(auto n = getProperty(ch, "imagePressed")){
			this->imgPressed = morda::Morda::inst().resMan.load<ResNinePatch>(n->value());
		}else{
			this->imgPressed = morda::Morda::inst().resMan.load<ResNinePatch>("morda_npt_button_pressed");
		}
	}
	
	//initialize nine-patch
	this->SimpleButton::onPressedChanged();
}

void SimpleButton::onPressedChanged(){
	this->PushButton::onPressedChanged();
	this->setNinePatch(this->isPressed() ? this->imgPressed : this->imgNormal);
}
