#pragma once

#include "../widget.hpp"

#include "../../res/cursor.hpp"

namespace morda{

/**
 * @brief Mouse cursor widget.
 * This widget displays mouse cursor.
 * From GUI script this widget can be instantiated as "mouse_cursor".
 *
 * @param cursor - reference to cursor resource.
 */
class mouse_cursor : virtual public widget{
	std::shared_ptr<const res::cursor> cursor;

	std::shared_ptr<const res::image::texture> quadTex;

	vector2 cursorPos;
public:
	mouse_cursor(std::shared_ptr<morda::context> c, const puu::forest& desc);

	mouse_cursor(const mouse_cursor&) = delete;
	mouse_cursor& operator=(const mouse_cursor&) = delete;

	void setCursor(std::shared_ptr<const res::cursor> cursor);

	bool on_mouse_move(const morda::vector2& pos, unsigned pointerID) override;

	void render(const morda::matrix4& matrix) const override;
};

}
