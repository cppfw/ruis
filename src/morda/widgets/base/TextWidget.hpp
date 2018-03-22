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
	
public:
	TextWidget(const TextWidget&) = delete;
	TextWidget& operator=(const TextWidget&) = delete;
	
	void setFont(std::shared_ptr<ResFont> font);
	
	const morda::Font& font()const{
		return this->font_v->font();
	}
	
	virtual void setText(std::u32string&& text) = 0;
	
	void setText(const std::string& text){
		this->setText(unikod::toUtf32(text));
	}
	
	void clear(){
		this->setText(std::u32string());
	}
	
	virtual std::u32string getText()const = 0;
	
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
	
	std::u32string text_v;
	
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
	void onFontChanged()override{
		this->recomputeBoundingBox();
	}

	void onTextChanged()override;
	
	using TextWidget::setText;
	
	void setText(std::u32string&& text)override;
	
	std::u32string getText()const override{
		return this->text_v;
	}
};


}
