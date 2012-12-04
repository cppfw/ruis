/* The MIT License:

Copyright (c) 2012 Ivan Gagis <igagis@gmail.com>

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



namespace morda{

namespace key{

enum Key{
	SPACE,
	ENTER,
	ZERO,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	COMMA,
	SEMICOLON,
	APOSTROPHE,
	PERIOD,
	SLASH,
	BACKSLASH,
	TAB,
	LEFT_SHIFT,
	RIGHT_SHIFT,
	END,
	LEFT_SQUARE_BRACKET,
	RIGHT_SQUARE_BRACKET,
	TICK,
	MINUS,
	EQUALS,
	BACKSPACE,
	CAPSLOCK,
	ESCAPE,
	LEFT_CONTROL,
	LEFT_ALT,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	RIGHT_CONTROL,
	PRINT_SCREEN,
	RIGHT_ALT,
	HOME,
	PAGE_UP,
	PAGE_DOWN,
	INSERT,
	DELETE,
	PAUSE,
	WINDOWS,
	WINDOWS_MENU
};

}//~namespace



class KeyListener{
public:
	virtual ~KeyListener()throw(){}
	
	//return true to consume
	virtual bool OnKeyDown(key::Key keyCode){
		return false;
	}
	
	//return true to consume
	virtual bool OnKeyUp(key::Key keyCode){
		return false;
	}
	
	/**
	 * @brief Handler of character input event.
	 * Handler for character input. Character input is only passed to focused widget.
	 * It will not be passed to root container if no widget is focused.
	 * Character input is not propagated to parent widgets. The consumption
	 * of the event only has effect when event is consumed in KeyListener set to a widget,
	 * then it will not be propagated to Widgets overridden handler method.
     * @param unicode - unicode value of entered character.
	 * @param isRepeated - true if the event is a result of auto-repeating when key is held pressed. False otherwise.
	 * @return true to consume event.
	 * @return false to allow event to propagate further.
     */
	virtual bool OnCharacterInput(ting::u32 unicode, bool isRepeated){
		return false;
	}
};



}//~namespace
