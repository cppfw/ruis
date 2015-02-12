/* The MIT License:

Copyright (c) 2014 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Widget.hpp"
#include "ColorWidget.hpp"

#include "../resources/ResFont.hpp"

#include "../util/Rectangle2.hpp"

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
	
	virtual ~TextWidget()NOEXCEPT{}
	
	void SetFont(std::shared_ptr<ResFont> font){
		if(!font){
			throw morda::Exc("TextWidget::SetFont(): passed argument is null");
		}
		
		this->font = std::move(font);
		
		this->SetRelayoutNeeded();
		
		this->OnFontChanged();
	}
	
	const morda::Font& Font()const{
		return this->font->font();
	}
	
	virtual void OnFontChanged(){}
	
protected:
	TextWidget(const stob::Node* desc);

private:

};


class SingleLineTextWidget : public TextWidget{
	std::vector<std::uint32_t> text;
	
	mutable Rect2r bb;
	
protected:
	Vec2r Measure(const morda::Vec2r& quotum)const NOEXCEPT override{
		return Vec2r(this->bb.d.x, this->Font().BoundingBox().d.y - this->Font().BoundingBox().p.y);
	}
	
	SingleLineTextWidget(const stob::Node* chain);
	
	const Rect2r& TextBoundingBox()const{
		return this->bb;
	}
	
	void RecomputeBoundingBox(){
		this->bb = this->Font().StringBoundingBox(this->text);
	}
public:
	
	void SetText(decltype(text)&& text){
		this->text = std::move(text);
		this->SetRelayoutNeeded();
		this->RecomputeBoundingBox();
	}
	

	void OnFontChanged()override{
		this->RecomputeBoundingBox();
	}

	
	decltype(text) Clear(){
		return std::move(this->text);
	}
	
	const decltype(text)& Text()const NOEXCEPT{
		return this->text;
	}
};


}
