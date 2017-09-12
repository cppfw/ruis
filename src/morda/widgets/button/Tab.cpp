#include "Tab.hpp"
#include "../../util/util.hpp"


using namespace morda;

void Tab::onCheckedChanged() {
	if(this->isChecked()){
		this->setNinePatch(this->activeNinePatch);
	}else{
		this->setNinePatch(this->inactiveNinePatch);
	}
	this->ChoiceButton::onCheckedChanged();
}


Tab::Tab(const stob::Node* chain) :
		Widget(chain),
		ChoiceButton(chain),
		NinePatch(chain)
{
	if(auto p = getProperty(chain, "look")){
		if(auto im = getProperty(p, "active")){
			this->activeNinePatch = morda::inst().resMan.load<ResNinePatch>(im->value());
		}
		if(auto im = getProperty(p, "inactive")){
			this->inactiveNinePatch = morda::inst().resMan.load<ResNinePatch>(im->value());
		}
	}
	if(!this->activeNinePatch){
		this->activeNinePatch = morda::inst().resMan.load<ResNinePatch>("morda_npt_tab_active");
	}
	if(!this->inactiveNinePatch){
		this->inactiveNinePatch = morda::inst().resMan.load<ResNinePatch>("morda_npt_tab_inactive");
	}
	
	this->onCheckedChanged();
}
