#pragma once

#include "../Widget.hpp"
#include "ColorWidget.hpp"

#include "../../res/ResFont.hpp"

#include <kolme/Rectangle.hpp>

#include <list>
#include <vector>

namespace morda{

/**
 * @brief Abstract widget displaying a text.
 */
//TODO: should not be color widget
class TextWidget : public ColorWidget{
	std::shared_ptr<ResFont> font_v;
	
	std::u32string text_v;
	
public:
	TextWidget(const TextWidget&) = delete;
	TextWidget& operator=(const TextWidget&) = delete;
	
	void setFont(std::shared_ptr<ResFont> font);
	
	const morda::Font& font()const{
		return this->font_v->font();
	}
	
	void setText(decltype(text_v)&& text){
		this->text_v = std::move(text);
		this->setRelayoutNeeded();
		this->onTextChanged();
	}
	
	void setText(const std::string& text){
		this->setText(unikod::toUtf32(text));
	}
	
	decltype(text_v) clear(){
		return std::move(this->text_v);
	}
	
	const decltype(text_v)& text()const noexcept{
		return this->text_v;
	}
	
	virtual void onFontChanged(){}
	
	virtual void onTextChanged(){
		if (this->textChanged) {
			this->textChanged(*this);
		}
	}
	
	std::function<void(TextWidget& w)> textChanged;
	
protected:
	TextWidget(const stob::Node* desc);

private:

};


class SingleLineTextWidget : public TextWidget{
	mutable Rectr bb;
	
protected:
	Vec2r measure(const morda::Vec2r& quotum)const noexcept override;
	
	SingleLineTextWidget(const stob::Node* chain);
	
	const Rectr& textBoundingBox()const{
		return this->bb;
	}
	
	void recomputeBoundingBox(){
		this->bb = this->font().stringBoundingBox(this->text());
	}
public:
	void onFontChanged()override{
		this->recomputeBoundingBox();
	}

	void onTextChanged()override;
};


}
