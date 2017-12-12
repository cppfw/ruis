#include "TextWidget.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"

using namespace morda;



TextWidget::TextWidget(const stob::Node* desc) :
		Widget(desc),
		ColorWidget(desc)
{
	if(const stob::Node* p = getProperty(desc, "font")){
		this->font_v = Morda::inst().resMan.load<morda::ResFont>(p->value());
	}else{
		this->font_v = morda::Morda::inst().resMan.load<ResFont>("morda_fnt_normal");
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
	Vec2r ret(this->bb.d);
	
	for(unsigned i = 0; i != ret.size(); ++i){
		if(quotum[i] >= 0){
			ret[i] = quotum[i];
		}
	}
	
	return ret;
}

void SingleLineTextWidget::onTextChanged() {
	if (this->textChanged) {
		this->textChanged(*this);
	}
}

