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

#include "spinning_image.hpp"

namespace ruis {

/**
 * @brief Refreshing image.
 *
 * A %spinning_image which loads a "refresh" icon by default.
 */
class refresh : public spinning_image
{
public:
	struct parameters {
		ruis::spinning_image::parameters spinning_image;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

	refresh(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);
};

namespace make {
inline utki::shared_ref<ruis::refresh> refresh(
	const utki::shared_ref<ruis::context>& context, //
	ruis::refresh::all_parameters params
)
{
	return utki::make_shared<ruis::refresh>(
		context, //
		std::move(params)
	);
}
} // namespace make

} // namespace ruis
