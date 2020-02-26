#pragma once

#include "../widget.hpp"

#include "../../res/res_cursor.hpp"

namespace morda{


/**
 * @brief Mouse cursor widget.
 * This widget displays mouse cursor.
 * From GUI script this widget can be instantiated as "MouseCursor".
 *
 * @param cursor - reference to cursor resource.
 */
class MouseCursor : virtual public widget{
	std::shared_ptr<const res_cursor> cursor;

	std::shared_ptr<const res_image::texture> quadTex;

	Vec2r cursorPos;
public:
	MouseCursor(std::shared_ptr<morda::context> c, const puu::forest& desc);

	MouseCursor(const MouseCursor&) = delete;
	MouseCursor& operator=(const MouseCursor&) = delete;

	void setCursor(std::shared_ptr<const res_cursor> cursor);

	bool on_mouse_move(const morda::Vec2r& pos, unsigned pointerID) override;

	void render(const morda::Matr4r& matrix) const override;
};

}
