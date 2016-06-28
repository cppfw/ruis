#include "Margins.hpp"
#include "../../../util/util.hpp"

using namespace morda;


Margins::Margins(const stob::Node* chain) :
		Widget(chain)
{
	{
		Sidesr m;

		if(auto n = getProperty(chain, "left")){
			m.left() = dimValueFromLayoutStob(*n);
		}else{
			m.left() = 0;
		}
		
		if(auto n = getProperty(chain, "right")){
			m.right() = dimValueFromLayoutStob(*n);
		}else{
			m.right() = 0;
		}
		
		if(auto n = getProperty(chain, "top")){
			m.top() = dimValueFromLayoutStob(*n);
		}else{
			m.top() = 0;
		}
		
		if(auto n = getProperty(chain, "bottom")){
			m.bottom() = dimValueFromLayoutStob(*n);
		}else{
			m.bottom() = 0;
		}
		
		this->setMargins(m);
	}
	
	//TODO:
}

void Margins::setWidget(std::shared_ptr<Widget> w){
	//TODO:
	
	this->child = std::move(w);
}


void Margins::setMargins(Sidesr margins){
	//TODO:
	this->margins_v = margins;
}
