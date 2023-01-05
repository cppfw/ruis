/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "../../render/renderer.hpp"

namespace morda{

/**
 * @brief Basic widget which allows configuring alpha blending when rendering.
 * @param blend - enable (true) or disable (false) alpha blending.
 * @param blend_src - source RGB components blend factor.
 * @param blend_dst - destination RGB components blend factor.
 * @param blend_src_alpha - source alpha component blend factor.
 * @param blend_dst_alpha - destination alpha component blend factor.
 *
 * List of possible blend factor values: zero, one, src_color, one_minus_src_color, dst_color, one_minus_dst_color, src_alpha,
 * one_minus_src_alpha, dst_alpha, one_minus_dst_alpha, constant_color, one_minus_constant_color, constant_alpha, one_minus_constant_alpha,
 * src_alpha_saturate.
 */
class blending_widget : public virtual widget{
	bool isBlendingEnabled_v = true;
public:
	/**
	 * @brief Structure holding blending parameters.
	 */
	struct blending_params{
		morda::renderer::blend_factor src;
		morda::renderer::blend_factor dst;
		morda::renderer::blend_factor src_alpha;
		morda::renderer::blend_factor dst_alpha;

		bool operator==(const blending_params& b){
			return this->src == b.src && this->dst == b.dst && this->src_alpha == b.src_alpha && this->dst_alpha == b.dst_alpha;
		}
	};
private:
	blending_params blend_v = {
		morda::renderer::blend_factor::src_alpha,
		morda::renderer::blend_factor::one_minus_src_alpha,
		morda::renderer::blend_factor::one,
		morda::renderer::blend_factor::one_minus_src_alpha
	};

protected:
	blending_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc);

public:
	blending_widget(const blending_widget&) = delete;
	blending_widget& operator=(const blending_widget&) = delete;

	/**
	 * @brief Set blending to rendering context.
	 */
	void set_blending_to_renderer()const;

	/**
	 * @brief Enable/disable blending for this widget.
	 */
	void set_blending_enabled(bool enable);

	/**
	 * @brief Check if blending is enabled for this widget.
	 */
	bool is_blending_enabled()const noexcept{
		return this->isBlendingEnabled_v;
	}

	/**
	 * @brief Set blending parameters for this widget.
	 * @param params - blending parameters to take into use.
	 */
	void set_blending_params(const blending_params& params);

	/**
	 * @brief Get blending parameters of this widget.
	 * @return Blending parameters of this widget.
	 */
	const blending_params& get_blending_params()const noexcept{
		return this->blend_v;
	}

	/**
	 * @brief Invoked when blending parameters are changed.
	 * Also invoked if blending enabled/disabled status has changed.
	 */
	virtual void on_blending_change(){}
};

}
