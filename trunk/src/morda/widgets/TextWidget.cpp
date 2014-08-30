#include "TextWidget.hpp"

#include "../App.hpp"


using namespace morda;


namespace{
std::shared_ptr<ResFont> DefaultFont(){
	return morda::App::Inst().resMan.Load<ResFont>("fnt_main");
}
}


TextWidget::TextWidget(const stob::Node* desc) :
		Widget(desc)
{
	if(!desc){
		this->font = DefaultFont();
	}
	
	if(const stob::Node* p = desc->GetProperty("font")){
		this->font = App::Inst().resMan.Load<morda::ResFont>(p->Value());
	}else{
		this->font = DefaultFont();
	}
}

void TextWidget::Render(const morda::Matr4r& matrix)const{
	//TODO: render multiple lines
	for(auto& l : this->lines){
		this->Font().RenderString(matrix, l);
	}
}


void TextWidget::SetLines(decltype(TextWidget::lines)&& lines){
	this->lines = std::move(lines);

	//TODO: recompute bounding box for multiple lines
	for(auto& l : this->lines){
		this->bb = this->Font().StringBoundingBox(l);
	}

	this->SetRelayoutNeeded();
}
