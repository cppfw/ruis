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

#include "../../render/renderer.hpp"
#include "../widget.hpp"

namespace ruis {

/**
 * @brief Basic widget which allows configuring alpha blending when rendering.
 * @param blend - enable (true) or disable (false) alpha blending.
 * @param blend_src - source RGB components blend factor.
 * @param blend_dst - destination RGB components blend factor.
 * @param blend_src_alpha - source alpha component blend factor.
 * @param blend_dst_alpha - destination alpha component blend factor.
 *
 * List of possible blend factor values: zero, one, src_color, one_minus_src_color, dst_color, one_minus_dst_color,
 * src_alpha, one_minus_src_alpha, dst_alpha, one_minus_dst_alpha, constant_color, one_minus_constant_color,
 * constant_alpha, one_minus_constant_alpha, src_alpha_saturate.
 */
class blending_widget : public virtual widget
{
public:
	/**
	 * @brief Structure holding blending parameters.
	 */
	struct blending_factors {
		ruis::render::context::blend_factor src;
		ruis::render::context::blend_factor dst;
		ruis::render::context::blend_factor src_alpha;
		ruis::render::context::blend_factor dst_alpha;

		bool operator==(const blending_factors& b)
		{
			return //
				this->src == b.src && //
				this->dst == b.dst && //
				this->src_alpha == b.src_alpha && //
				this->dst_alpha == b.dst_alpha;
		}
	};

public:
	struct parameters {
		bool enabled = true;
		blending_factors factors = {
			ruis::render::context::blend_factor::src_alpha,
			ruis::render::context::blend_factor::one_minus_src_alpha,
			ruis::render::context::blend_factor::one,
			ruis::render::context::blend_factor::one_minus_src_alpha
		};
	};

private:
	parameters params;

protected:
	blending_widget(
		utki::shared_ref<ruis::context> context, //
		parameters params
	);

public:
	blending_widget(const blending_widget&) = delete;
	blending_widget& operator=(const blending_widget&) = delete;

	blending_widget(blending_widget&&) = delete;
	blending_widget& operator=(blending_widget&&) = delete;

	~blending_widget() override = default;

	/**
	 * @brief Set blending to rendering context.
	 */
	void set_blending_to_renderer() const;

	/**
	 * @brief Enable/disable blending for this widget.
	 */
	void set_blending_enabled(bool enable);

	/**
	 * @brief Check if blending is enabled for this widget.
	 */
	bool is_blending_enabled() const noexcept
	{
		return this->params.enabled;
	}

	/**
	 * @brief Set blending parameters for this widget.
	 * @param params - blending parameters to take into use.
	 */
	void set_blending_factors(const blending_factors& params);

	/**
	 * @brief Get blending parameters of this widget.
	 * @return Blending parameters of this widget.
	 */
	const blending_factors& get_blending_factors() const noexcept
	{
		return this->params.factors;
	}

	/**
	 * @brief Invoked when blending parameters are changed.
	 * Also invoked if blending enabled/disabled status has changed.
	 */
	virtual void on_blending_change() {}
};

} // namespace ruis
