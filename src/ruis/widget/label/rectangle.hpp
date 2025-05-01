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

#include "../base/color_widget.hpp"

#include "padding.hpp"

namespace ruis {

/**
 * @brief Rectangle widget.
 * This is a widget which can display a rectangle of a single color.
 */
class rectangle :
	public padding, //
	public color_widget
{
public:
	struct parameters {
		bool rounded_corners = false;
	};

private:
	parameters params;

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		padding::parameters padding_params;
		color_widget::parameters color_params;
		parameters rectangle_params;
	};

	rectangle(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

	rectangle(const rectangle&) = delete;
	rectangle& operator=(const rectangle&) = delete;

	rectangle(rectangle&&) = delete;
	rectangle& operator=(rectangle&&) = delete;

	~rectangle() override = default;

	void render(const ruis::matrix4& matrix) const override;

private:
	void render_rounder_corners(const mat4& matrix) const;

	struct rounded_corners_texture {
		utki::shared_ref<const render::texture_2d> tex;

		// vaos for corners
		std::array<std::array<utki::shared_ref<const render::vertex_array>, 2>, 2> vaos;

		rounded_corners_texture(
			ruis::render::renderer& r, //
			utki::shared_ref<const render::texture_2d> tex,
			vec2 middle
		);

	private:
	};

	static std::map<
		sides<real>, //
		std::weak_ptr<rounded_corners_texture>,
		sides<real>::comparator //
		>
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, "false-positive")
		cache;

	std::shared_ptr<rounded_corners_texture> rounded_corners_tex;

	void update_rounded_corners_texture();
};

namespace make {
inline utki::shared_ref<ruis::rectangle> rectangle(
	utki::shared_ref<ruis::context> context,
	rectangle::all_parameters params,
	widget_list children = {}
)
{
	return utki::make_shared<ruis::rectangle>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}

} // namespace make

} // namespace ruis
