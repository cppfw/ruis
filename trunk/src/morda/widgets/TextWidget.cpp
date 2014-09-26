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
		Widget(desc)
{
	if(const stob::Node* p = GetProperty(desc, "font")){
		this->font = App::Inst().resMan.Load<morda::ResFont>(p->Value());
	}else{
		this->font = DefaultFont();
	}
	
	if(const stob::Node* p = GetProperty(desc, "color")){
		this->color = p->AsUint32();
	}else{
		this->color = 0xffffffff;
	}
}



SingleLineTextWidget::SingleLineTextWidget(const stob::Node* desc) :
		Widget(desc),
		TextWidget(desc)
{
	if(!desc){
		return;
	}

	if(const stob::Node* p = desc->GetProperty("text")){
		this->SetText(ting::utf8::ToUTF32(p->Value()));
	}
}

