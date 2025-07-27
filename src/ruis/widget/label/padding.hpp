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

#include "../../util/content_wrapping.hpp"
#include "../container.hpp"

namespace ruis {

// Even though container is inherited privately, the std::enable_shared_from_this base is still accessable via
// public inheritance of widget. So, the linter complaint is false-positive, suppress it.
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "false-positive")
class padding :
	public virtual widget, //
	public content_wrapping,
	protected container
{
public:
	struct parameters {
		sides<length> borders;
	};

private:
	parameters params;

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		parameters padding_params;
	};

	padding(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

public:
	vec2 measure(const vec2& quotum) const override;
	void on_lay_out() override;

	/**
	 * @brief Set border settings.
	 * @param borders - border values to set.
	 */
	void set_borders(sides<length> borders);

	/**
	 * @brief Get current border settings.
	 * @return Current borders.
	 */
	const decltype(params.borders)& get_borders() const noexcept
	{
		return this->params.borders;
	}

	sides<real> get_actual_borders() const noexcept;

	virtual void on_borders_change();

protected:
	virtual sides<real> get_min_borders() const noexcept;
};

namespace make {
/**
 * @brief Construct 'padding' widget.
 * Default layout is pile.
 * @param context - ruis context.
 * @param params - 'padding' widget parameters.
 * @param children - children of the constructed 'padding' widget.
 * @return newly constructed 'padding' widget.
 */
utki::shared_ref<ruis::padding> padding(
	utki::shared_ref<context> context, //
	padding::all_parameters params,
	widget_list children
);
} // namespace make

} // namespace ruis
