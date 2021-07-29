/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#pragma once

#include "../group/pile.hpp"

#include "../../res/cursor.hpp"

namespace morda{

/**
 * @brief Mouse cursor widget.
 * This widget displays mouse cursor.
 * From GUI script this widget can be instantiated as "image_mouse_cursor".
 *
 * @param cursor - reference to cursor resource.
 */
class image_mouse_cursor : public pile{
	std::shared_ptr<const res::cursor> cursor;

	std::shared_ptr<const res::image::texture> quadTex;

	vector2 cursorPos;
public:
	image_mouse_cursor(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	image_mouse_cursor(const image_mouse_cursor&) = delete;
	image_mouse_cursor& operator=(const image_mouse_cursor&) = delete;

	void setCursor(std::shared_ptr<const res::cursor> cursor);

	bool on_mouse_move(const mouse_move_event& e)override;

	void render(const morda::matrix4& matrix) const override;
};

}
