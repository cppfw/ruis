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
#include "TextWidget.hpp"

#include "../Updateable.hpp"
#include "CharInputWidget.hpp"


namespace morda{

class TextInput : public SingleLineTextWidget, public Updateable, public CharInputWidget{
	
	size_t firstVisibleCharIndex = 0;
	real xOffset = 0;
	
	real cursorPos;
	
	size_t cursorIndex = 0;
	
	real selectionStartPos;
	
	size_t selectionStartIndex = 0;
	
	bool cursorBlinkVisible;
	
	bool ctrlPressed;
	bool shiftPressed;
	
	bool leftMouseButtonDown = false;
	
public:
	TextInput(const TextInput&) = delete;
	TextInput& operator=(const TextInput&) = delete;
	
	TextInput(const stob::Node* chain = nullptr);
	
	virtual ~TextInput()noexcept{}

	Vec2r measure(const morda::Vec2r& quotum)const noexcept override;

	void render(const morda::Matr4r& matrix) const override;

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)override;
	
	bool onMouseMove(const morda::Vec2r& pos, unsigned pointerId)override;

	void onFocusedChanged()override;
	
	bool onKey(bool isDown, EKey keyCode)override;
	
	void onResize()override;
	
	void Update(std::uint32_t dt)override;
	
	void OnCharacterInput(const utki::Buf<std::uint32_t> unicode, EKey key)override;

	void SetCursorIndex(size_t index, bool selection = false);
	
private:
	void UpdateCursorPosBasedOnIndex();
	
	void StartCursorBlinking();
	
	size_t PosToIndex(real pos);
	
	real IndexToPos(size_t index);
	
	bool ThereIsSelection()const noexcept{
		return this->cursorIndex != this->selectionStartIndex;
	}
	
	//returns new cursor index
	size_t DeleteSelection();
};

}
