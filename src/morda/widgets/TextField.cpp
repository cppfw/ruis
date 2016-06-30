#include "TextField.hpp"

#include "../App.hpp"
#include "../util/util.hpp"


using namespace morda;



TextField::TextField(const stob::Node* chain) :
		Widget(chain),
		ti(utki::makeShared<TextInput>(chain))
{
	if(auto n = getProperty(chain, "background")){
		this->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>(n->value()));
	}else{
		this->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>("morda_npt_textfield_background"));
	}
	
	this->content().add(this->ti);
	
	{
		auto& lp = this->content().getLayoutParams(*this->ti);
		lp.dim.x = Widget::LayoutParams::fill_c;
		lp.dim.y = Widget::LayoutParams::min_c;
	}
}
