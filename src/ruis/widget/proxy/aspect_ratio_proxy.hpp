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

#include "../widget.hpp"

namespace ruis {

/**
 * @brief A proxy widget which tries to preserve the aspect ratio of its own
 * dimensions.
 */
class aspect_ratio_proxy : virtual public widget
{
public:
	/**
	 * @brief Aspect ratio proxy specific parameters.
	 */
	struct parameters {
		/**
		 * @brief Aspect ratio, i.e. x dimension divided by y dimension.
		 */
		real x_above_y = 1;
	};

private:
	parameters params;

public:
	/**
	 * @brief All aspect ratio proxy parameters.
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
		 * @brief Aspect ratio proxy specific parameters.
		 */
		parameters aspect_ratio_params;
	};

	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param params - aspect ratio proxy parameters.
	 */
	aspect_ratio_proxy(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params
	);

	aspect_ratio_proxy(const aspect_ratio_proxy&) = delete;
	aspect_ratio_proxy& operator=(const aspect_ratio_proxy&) = delete;

	aspect_ratio_proxy(aspect_ratio_proxy&&) = delete;
	aspect_ratio_proxy& operator=(aspect_ratio_proxy&&) = delete;

	/**
	 * @brief Default destructor.
	 */
	~aspect_ratio_proxy() override = default;

	/**
	 * @brief Set aspect ratio.
	 * @param x_above_y - aspect ratio, i.e. x dimension divided by y dimension.
	 */
	void set_aspect_ratio(real x_above_y);

	/**
	 * @brief Get aspect ratio.
	 * @return aspect ratio, i.e. x dimension divided by y dimension.
	 */
	real get_aspect_ratio() const noexcept
	{
		return this->params.x_above_y;
	}

	/**
	 * @brief Measure how big the widget wants to be.
	 * @param quotum - space available to the widget.
	 * @param mode - measurement mode for each dimension.
	 * @return Measured desired widget dimensions.
	 */
	ruis::vec2 measure(
		const ruis::vec2& quotum, //
		const r4::vector2<measure_mode>& mode
	) const override;
};

namespace make {
/**
 * @brief Construct an aspect ratio proxy widget.
 * @param context - ruis context.
 * @param params - aspect ratio proxy parameters.
 * @return newly constructed aspect ratio proxy widget.
 */
inline utki::shared_ref<ruis::aspect_ratio_proxy> aspect_ratio_proxy(
	const utki::shared_ref<ruis::context>& context, //
	ruis::aspect_ratio_proxy::all_parameters params
)
{
	return utki::make_shared<ruis::aspect_ratio_proxy>(
		context, //
		std::move(params)
	);
}
} // namespace make

} // namespace ruis
