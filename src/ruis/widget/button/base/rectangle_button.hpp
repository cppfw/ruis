/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include "../../label/rectangle.hpp"

#include "virtual/button.hpp"

namespace ruis {

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "false positive")
class rectangle_button :
	virtual public button, //
	// no reason to make rectangle inheritance private
	public rectangle
{
public:
	struct specific_parameters {
		styled<ruis::color> pressed_color;
		styled<ruis::color> unpressed_color;
		styled<ruis::color> disabled_color;
		styled<ruis::color> pressed_stroke_color;
		styled<ruis::color> unpressed_stroke_color;
		styled<ruis::color> disabled_stroke_color;
		styled<ruis::length> stroke_width;
		styled<ruis::length> disabled_stroke_width;
	};

	struct parameters {
		ruis::rectangle::parameters rectangle;
		specific_parameters specific;
	};

	void render(const mat4& matrix) const override;

private:
	specific_parameters params;

	void update_color();

protected:
	void on_pressed_change() override;

	rectangle_button( //
		const utki::shared_ref<ruis::context>& context,
		parameters params,
		widget_list contents //
	);
};

} // namespace ruis
