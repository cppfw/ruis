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

#include "../base/containing_widget.hpp"
#include "../container.hpp"

namespace ruis {

// Even though container is inherited privately, the std::enable_shared_from_this base is still accessable via
// public inheritance of widget. So, the linter complaint is false-positive, suppress it.
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "false-positive")
class padding :
	// Explicit virtual public inheritance of widget at the first place is needed
	// to prevent it to be protected by private inheritance of container.
	// Otherwise, the compiler complains hen trying to use std::enable_shared_from_this<widget> base of widget:
	//   fatal error: 'shared_from_this' is a protected member of 'std::enable_shared_from_this<utki::shared>'
	//   note: constrained by protected inheritance here: protected container
	virtual public widget, //
	protected container,
	public containing_widget
{
public:
	/**
	 * @brief Padding widget specific parameters.
	 */
	struct specific_parameters {
		/**
		 * @brief Padding values for each side.
		 */
		sides<styled<length>> borders;
	};

	/**
	 * @brief Padding widget parameters.
	 */
	struct parameters {
		/**
		 * @brief Container parameters.
		 */
		ruis::container::parameters container;

		/**
		 * @brief Padding widget specific parameters.
		 */
		specific_parameters specific;
	};

private:
	specific_parameters params;

public:
	/**
	 * @brief All padding widget parameters.
	 */
	struct all_parameters {
		/**
		 * @brief Layout parameters.
		 */
		ruis::layout_parameters layout_params;

		/**
		 * @brief Base widget parameters.
		 */
		ruis::widget::parameters widget;

		/**
		 * @brief Padding widget parameters.
		 */
		parameters params;
	};

private:
	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param params - padding widget parameters.
	 * @param content_container - content container for the padding widget.
	 */
	padding(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters& params,
		utki::shared_ref<ruis::container> content_container
	);

public:
	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param params - padding widget parameters.
	 * @param children - children of the padding widget.
	 */
	padding(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params,
		widget_list children
	);

public:
	/**
	 * @brief Measure how big the widget wants to be.
	 * @param quotum - space available to the widget.
	 * @param mode - measurement mode for each dimension.
	 * @return Measured desired widget dimensions.
	 */
	vec2 measure(
		const vec2& quotum, //
		const r4::vector2<measure_mode>& mode
	) const override;

	/**
	 * @brief Invoked when the widget needs to lay out its contents.
	 */
	void on_lay_out() override;

	/**
	 * @brief Set border settings.
	 * @param borders - border values to set.
	 */
	void set_borders(sides<styled<length>> borders);

	/**
	 * @brief Get current border settings.
	 * @return Current borders.
	 */
	const decltype(params.borders)& get_borders() const noexcept
	{
		return this->params.borders;
	}

	/**
	 * @brief Get the actual borders in physical pixels.
	 * The actual borders are the borders computed from the current border
	 * settings, i.e. the padding actually applied around the contained content.
	 * @return actual borders in physical pixels.
	 */
	sides<real> get_actual_borders() const noexcept;

	/**
	 * @brief Invoked when the borders have changed.
	 */
	virtual void on_borders_change();

protected:
	/**
	 * @brief Get minimum borders.
	 * @return minimum borders to be kept around the contained widgets.
	 */
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
	const utki::shared_ref<context>& context, //
	padding::all_parameters params,
	widget_list children
);
} // namespace make

} // namespace ruis
