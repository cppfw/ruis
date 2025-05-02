/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "button.hpp"

namespace ruis {

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "false positive")
class rectangle_button :
	virtual public button, //
	// no reason to make rectangle inheritance private
	public rectangle
{
public:
	struct parameters {
		constexpr static const auto default_pressed_color = 0xffaaaaaa;
		uint32_t pressed_color = default_pressed_color;

		constexpr static const auto default_unpressed_color = 0xff666666;
		uint32_t unpressed_color = default_unpressed_color;

		// TODO: add disabled_color ?
	};

	void render(const matrix4& matrix) const override;

private:
	parameters params;

	void update_color();

protected:
	void on_pressed_change() override;

	rectangle_button( //
		utki::shared_ref<ruis::context> context,
        container::parameters container_params,
		padding::parameters padding_params,
		rectangle::parameters rectangle_params,
		parameters params,
        widget_list contents
	);
};

} // namespace ruis
