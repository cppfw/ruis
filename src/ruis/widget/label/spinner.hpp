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

#include "../../res/image.hpp"
#include "../../updateable.hpp"

#include "image.hpp"

namespace ruis {

/**
 * @brief Spinning image label.
 * @param active - whether the spinner is initially active or not, can be true/false.
 */
class spinner :
	public image, //
	public updateable
{
	real angle = 0;

public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		image::parameters image_params;
		blending_widget::parameters blending_params;
	};

	spinner(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	void set_active(bool active);

	void render(const matrix4& matrix) const override;

private:
	void update(uint32_t dt_ms) override;
};

namespace make {
inline utki::shared_ref<ruis::spinner> spinner(
	utki::shared_ref<ruis::context> context, //
	spinner::all_parameters params
)
{
	return utki::make_shared<ruis::spinner>(
		std::move(context), //
		std::move(params)
	);
}

utki::shared_ref<ruis::spinner> refresh(
	utki::shared_ref<ruis::context> context, //
	spinner::all_parameters params
);

} // namespace make

} // namespace ruis
