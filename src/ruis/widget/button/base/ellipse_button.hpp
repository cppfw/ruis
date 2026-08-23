/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#pragma once

#include "../../label/ellipse.hpp"

#include "button.hpp"

namespace ruis {

class ellipse_button :
	virtual public button, //
	public ellipse
{
public:
	struct parameters {
		styled<ruis::color> pressed_color;
		styled<ruis::color> unpressed_color;
	};

	void render(const mat4& matrix) const override;

private:
	parameters params;

	void update_color();

protected:
	void on_pressed_change() override;

	ellipse_button( //
		utki::shared_ref<ruis::context> context,
		container::parameters container_params,
		padding::parameters padding_params,
		parameters params,
		widget_list contents //
	);
};

} // namespace ruis