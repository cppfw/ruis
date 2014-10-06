#include "TextWidget.hpp"

#include "../App.hpp"

#include "../util/util.hpp"

using namespace morda;


namespace{
std::shared_ptr<ResFont> DefaultFont(){
	return morda::App::Inst().resMan.Load<ResFont>("morda_fnt_main");
}
}


TextWidget::TextWidget(const stob::Node* desc) :
		Widget(desc),
		ColorWidget(desc)
{
	if(const stob::Node* p = GetProperty(desc, "font")){
		this->font = App::Inst().resMan.Load<morda::ResFont>(p->Value());
	}else{
		this->font = DefaultFont();
	}
}



SingleLineTextWidget::SingleLineTextWidget(const stob::Node* chain) :
		Widget(chain),
		TextWidget(chain)
{
	if(auto p = GetProperty(chain, "text")){
		this->SetText(ting::utf8::ToUTF32(p->Value()));
	}
}

