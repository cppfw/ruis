/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Widget.hpp"
#include "ColorWidget.hpp"

#include "../resources/ResFont.hpp"

#include "../util/Rectangle.hpp"

#include <list>
#include <vector>

namespace morda{

/**
 * @brief Abstract widget displaying a text.
 */
class TextWidget : public ColorWidget{
	std::shared_ptr<ResFont> font;
	
public:
	TextWidget() = delete;
	TextWidget(const TextWidget&) = delete;
	TextWidget& operator=(const TextWidget&) = delete;
	
	virtual ~TextWidget()noexcept{}
	
	void SetFont(std::shared_ptr<ResFont> font){
		if(!font){
			throw morda::Exc("TextWidget::SetFont(): passed argument is null");
		}
		
		this->font = std::move(font);
		
		this->setRelayoutNeeded();
		
		this->onFontChanged();
	}
	
	const morda::Font& Font()const{
		return this->font->font();
	}
	
	virtual void onFontChanged(){}
	
protected:
	TextWidget(const stob::Node* desc);

private:

};


class SingleLineTextWidget : public TextWidget{
	std::vector<std::uint32_t> text_var;
	
	mutable Rectr bb;
	
protected:
	Vec2r measure(const morda::Vec2r& quotum)const noexcept override;
	
	SingleLineTextWidget(const stob::Node* chain);
	
	const Rectr& textBoundingBox()const{
		return this->bb;
	}
	
	void recomputeBoundingBox(){
		this->bb = this->Font().StringBoundingBox(utki::wrapBuf(this->text_var));
	}
public:
	
	void setText(decltype(text_var)&& text){
		this->text_var = std::move(text);
		this->setRelayoutNeeded();
		this->recomputeBoundingBox();
	}
	
	void setText(const std::string& text){
		this->setText(unikod::toUtf32(text));
	}

	void onFontChanged()override{
		this->recomputeBoundingBox();
	}

	
	decltype(text_var) clear(){
		return std::move(this->text_var);
	}
	
	const decltype(text_var)& text()const noexcept{
		return this->text_var;
	}
};


}
