#pragma once

#include "../widget.hpp"
#include "../base/TextWidget.hpp"

#include "../../Updateable.hpp"
#include "../CharInputWidget.hpp"


namespace morda{

/**
 * @brief TODO:.
 */
class TextInputLine :
		public SingleLineTextWidget,
		private Updateable,
		public CharInputWidget
{
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
	TextInputLine(const TextInputLine&) = delete;
	TextInputLine& operator=(const TextInputLine&) = delete;

	TextInputLine(const stob::Node* chain);

	virtual ~TextInputLine()noexcept{}

	Vec2r measure(const morda::Vec2r& quotum)const noexcept override;

	void render(const morda::Matr4r& matrix) const override;

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId)override;

	bool onMouseMove(const morda::Vec2r& pos, unsigned pointerId)override;

	void onFocusChanged()override;

	bool onKey(bool isDown, Key_e keyCode)override;

	void onResize()override;

	void update(std::uint32_t dt)override;

	void onCharacterInput(const std::u32string& unicode, Key_e key)override;

	void setCursorIndex(size_t index, bool selection = false);

private:
	void updateCursorPosBasedOnIndex();

	void startCursorBlinking();

	size_t posToIndex(real pos);

	real indexToPos(size_t index);

	bool thereIsSelection()const noexcept{
		return this->cursorIndex != this->selectionStartIndex;
	}

	//returns new cursor index
	size_t deleteSelection();
};

}
