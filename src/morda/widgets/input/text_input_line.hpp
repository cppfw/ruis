#pragma once

#include "../widget.hpp"
#include "../base/text_widget.hpp"

#include "../../updateable.hpp"
#include "../character_input_widget.hpp"

namespace morda{

/**
 * @brief TODO:.
 */
class text_input_line :
		public single_line_text_widget,
		public character_input_widget,
		private updateable
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
	text_input_line(const text_input_line&) = delete;
	text_input_line& operator=(const text_input_line&) = delete;

	text_input_line(std::shared_ptr<morda::context> c, const puu::forest& desc);

	virtual ~text_input_line()noexcept{}

	Vec2r measure(const morda::Vec2r& quotum)const noexcept override;

	void render(const morda::matrix4& matrix) const override;

	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerId)override;

	bool on_mouse_move(const morda::Vec2r& pos, unsigned pointerId)override;

	void on_focus_changed()override;

	bool on_key(bool isDown, key keyCode)override;

	void on_resize()override;

	void update(std::uint32_t dt)override;

	void on_character_input(const std::u32string& unicode, key keycode)override;

	void set_cursor_index(size_t index, bool selection = false);

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
