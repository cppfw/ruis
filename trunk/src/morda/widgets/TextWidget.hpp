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
#include "../resources/ResFont.hpp"

#include <list>
#include <vector>

namespace morda{

/**
 * @brief Abstract widget displaying a text.
 */
class TextWidget : public virtual Widget{
	std::shared_ptr<ResFont> font;
	
	std::list<std::vector<std::uint32_t>> lines;
	
	morda::Rect2f bb;//text bounding box
public:
	TextWidget() = delete;
	TextWidget(const TextWidget&) = delete;
	TextWidget& operator=(const TextWidget&) = delete;
	
	virtual ~TextWidget()NOEXCEPT{}
	
	void SetFont(std::shared_ptr<ResFont> font){
		ASSERT(font)
		this->font = font;
		
		this->SetRelayoutNeeded();
	}
	
	const morda::Font& Font()const{
		return this->font->font();
	}
	
	Vec2r ComputeMinDim()const NOEXCEPT override{
		return this->bb.d;
	}
	
	void Render(const morda::Matr4r& matrix)const override;
	
protected:
	TextWidget(const stob::Node* desc);
	
	const morda::Rect2f& TextBoundingBox(){
		return this->bb;
	}
	
	decltype(TextWidget::lines) ResetLines(){
		return std::move(this->lines);
	}
	
	void SetLines(decltype(TextWidget::lines)&& lines);
	
	const decltype(TextWidget::lines)& Lines(){
		return this->lines;
	}
private:

};

}
