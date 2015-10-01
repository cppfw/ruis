#include "TextWidget.hpp"

#include "../App.hpp"

#include "../util/util.hpp"

using namespace morda;


namespace{
std::shared_ptr<ResFont> DefaultFont(){
	return morda::App::inst().resMan.Load<ResFont>("morda_fnt_main");
}
}


TextWidget::TextWidget(const stob::Node* desc) :
		Widget(desc),
		ColorWidget(desc)
{
	if(const stob::Node* p = getProperty(desc, "font")){
		this->font = App::inst().resMan.Load<morda::ResFont>(p->value());
	}else{
		this->font = DefaultFont();
	}
}



SingleLineTextWidget::SingleLineTextWidget(const stob::Node* chain) :
		Widget(chain),
		TextWidget(chain)
{
	if(auto p = getProperty(chain, "text")){
		this->setText(unikod::toUtf32(p->value()));
	}
}



Vec2r SingleLineTextWidget::measure(const morda::Vec2r& quotum)const noexcept{
	Vec2r ret(this->bb.d.x, this->Font().BoundingBox().d.y - this->Font().BoundingBox().p.y);
	
	for(unsigned i = 0; i != ret.size(); ++i){
		if(quotum[i] >= 0){
			ret[i] = quotum[i];
		}
	}
	
	return ret;
}

