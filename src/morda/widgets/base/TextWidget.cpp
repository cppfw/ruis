#include "TextWidget.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

using namespace morda;

void TextWidget::setFont(std::shared_ptr<ResFont> font) {
	if (!font) {
		throw morda::Exc("TextWidget::SetFont(): passed argument is null");
	}

	this->font_v = std::move(font);

	this->invalidate_layout();

	this->onFontChanged();
}


TextWidget::TextWidget(const puu::forest& desc) :
		widget(desc),
		ColorWidget(desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "font"){
			this->font_v = gui::inst().resMan.load<morda::ResFont>(get_property_value(p).to_string().c_str());
		}
	}

	// load default font if needed
	if(!this->font_v){
		this->font_v = morda::gui::inst().resMan.load<ResFont>("morda_fnt_normal");
	}
}



SingleLineTextWidget::SingleLineTextWidget(const puu::forest& desc) :
		widget(desc),
		TextWidget(desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "text"){
			this->setText(unikod::toUtf32(get_property_value(p).to_string()));
		}
	}
}



Vec2r SingleLineTextWidget::measure(const morda::Vec2r& quotum)const noexcept{
	Vec2r ret(this->bb.d.x, this->font().height());

	for(unsigned i = 0; i != ret.size(); ++i){
		if(quotum[i] >= 0){
			ret[i] = quotum[i];
		}
	}

	return ret;
}

void SingleLineTextWidget::onTextChanged() {
	this->recomputeBoundingBox();
	this->TextWidget::onTextChanged();
}

void TextWidget::setText(std::u32string&& text) {
	this->lines.clear();
	this->lines.emplace_back(std::move(text));
	this->invalidate_layout();
	this->onTextChanged();
}

std::u32string TextWidget::getText() const {
	std::u32string ret;

	for(auto& l : this->lines){
		ret.append(l);
	}

	return ret;
}
