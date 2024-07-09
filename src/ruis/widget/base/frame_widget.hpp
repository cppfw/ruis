/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "../container.hpp"

namespace ruis {

class frame_widget :
	public virtual widget, //
	protected container
{
public:
	struct parameters {
		sides<length> borders;
	};

private:
	parameters params;

	container& inner_content;

protected:
	struct all_parameters {
		container::parameters container_params;
		parameters frame_params;
	};

	frame_widget(
		utki::shared_ref<ruis::context> context,
		all_parameters params,
		utki::span<const utki::shared_ref<widget>> frame_layout,
		utki::span<const utki::shared_ref<widget>> children
	);

public:
	/**
	 * @brief Get content container.
	 * @return The content container. This is where the child widgets are stored.
	 */
	container& content()
	{
		return this->inner_content;
	}

	/**
	 * @brief Set border settings.
	 * Border values are in pixels or -1 for minimal size.
	 * @param borders - border values to set.
	 */
	void set_borders(sides<length> borders);

	/**
	 * @brief Get current border settings.
	 * Border values are in pixels or min_c.
	 * @return Current borders.
	 */
	decltype(params.borders) get_borders() const noexcept
	{
		return this->params.borders;
	}

	virtual void on_borders_change() {}
};

} // namespace ruis
