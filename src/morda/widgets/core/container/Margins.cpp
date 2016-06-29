#include "Margins.hpp"

#include "../../../util/util.hpp"
#include "../../../App.hpp"

using namespace morda;


Margins::Margins(const stob::Node* chain) :
		Widget(chain)
{
	if(chain){
		if(auto n = chain->thisOrNextNonProperty().node()){
			this->child = morda::App::inst().inflater.inflate(*n);
			
			if(n->nextNonProperty().node()){
				throw morda::Exc("Margins::Margins(): only one child allowed for Margins");
			}
		}
	}
	
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
}

void Margins::setWidget(std::shared_ptr<Widget> w){
	this->child = std::move(w);
	this->setMargins(this->margins_v);
}


void Margins::setMargins(Sidesr margins){
	this->margins_v = margins;
	
	this->removeAll();
	
	if(!this->child){
		return;
	}
	
	if(this->margins_v.left() != 0){
		auto w = utki::makeShared<Widget>(nullptr);
		this->add(w);
		auto& lp = this->getLayoutParams(*w);
		lp.dim.x = this->margins_v.left();
		lp.dim.y = 0;
	}
	
	if(this->margins_v.top() != 0 || this->margins_v.bottom() != 0){
		auto va  = utki::makeShared<VerticalArea>();
		this->add(va);
		
		if(this->margins_v.top() != 0){
			auto w = utki::makeShared<Widget>(nullptr);
			va->add(w);
			auto& lp = va->getLayoutParams(*w);
			lp.dim.x = 0;
			lp.dim.y = this->margins_v.top();
		}
		
		va->add(this->child);
		
		if(this->margins_v.bottom() != 0){
			auto w = utki::makeShared<Widget>(nullptr);
			va->add(w);
			auto& lp = va->getLayoutParams(*w);
			lp.dim.x = 0;
			lp.dim.y = this->margins_v.bottom();
		}
	}else{
		this->add(this->child);
	}
	
	if(this->margins_v.right() != 0){
		auto w = utki::makeShared<Widget>(nullptr);
		this->add(w);
		auto& lp = this->getLayoutParams(*w);
		lp.dim.x = this->margins_v.right();
		lp.dim.y = 0;
	}
}
