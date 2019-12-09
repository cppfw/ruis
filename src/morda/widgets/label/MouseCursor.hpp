#pragma once

#include "../widget.hpp"

#include "../../res/ResCursor.hpp"

namespace morda{


/**
 * @brief Mouse cursor widget.
 * This widget displays mouse cursor.
 * From GUI script this widget can be instantiated as "MouseCursor".
 *
 * @param cursor - reference to cursor resource.
 */
class MouseCursor : virtual public Widget{
	std::shared_ptr<const ResCursor> cursor;

	std::shared_ptr<const ResImage::QuadTexture> quadTex;

	Vec2r cursorPos;
public:
	MouseCursor(const stob::Node* chain = nullptr);

	MouseCursor(const MouseCursor&) = delete;
	MouseCursor& operator=(const MouseCursor&) = delete;

	void setCursor(std::shared_ptr<const ResCursor> cursor);

	bool onMouseMove(const morda::Vec2r& pos, unsigned pointerID) override;

	void render(const morda::Matr4r& matrix) const override;
};

}
