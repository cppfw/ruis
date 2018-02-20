#pragma once

#include "../core/Widget.hpp"
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
	
public:
	TextWidget(const TextWidget&) = delete;
	TextWidget& operator=(const TextWidget&) = delete;
	
	void setFont(std::shared_ptr<ResFont> font){
		if(!font){
			throw morda::Exc("TextWidget::SetFont(): passed argument is null");
		}
		
		this->font_v = std::move(font);
		
		this->setRelayoutNeeded();
		
		this->onFontChanged();
	}
	
	const morda::Font& font()const{
		return this->font_v->font();
	}
	
	virtual void onFontChanged(){}
	
protected:
	TextWidget(const stob::Node* desc);

private:

};


class SingleLineTextWidget : public TextWidget{
	std::u32string text_v;
	
	mutable Rectr bb;
	
protected:
	Vec2r measure(const morda::Vec2r& quotum)const noexcept override;
	
	SingleLineTextWidget(const stob::Node* chain);
	
	const Rectr& textBoundingBox()const{
		return this->bb;
	}
	
	void recomputeBoundingBox(){
		this->bb = this->font().stringBoundingBox(this->text_v);
	}
public:
	
	void setText(decltype(text_v)&& text){
		this->text_v = std::move(text);
		this->setRelayoutNeeded();
		this->recomputeBoundingBox();
		this->onTextChanged();
	}
	
	void setText(const std::string& text){
		this->setText(unikod::toUtf32(text));
	}

	void onFontChanged()override{
		this->recomputeBoundingBox();
	}

	virtual void onTextChanged();
	
	std::function<void(SingleLineTextWidget& w)> textChanged;
	
	decltype(text_v) clear(){
		return std::move(this->text_v);
	}
	
	const decltype(text_v)& text()const noexcept{
		return this->text_v;
	}
};


}
