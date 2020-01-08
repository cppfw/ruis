#pragma once

#include "../widget.hpp"

#include "../../render/Renderer.hpp"

namespace morda{

/**
 * @brief Basic widget which allows configuring alpha blending when rendering.
 * @param blend - enable (true) or disable (false) alpha blending.
 * @param blendSrc - source RGB components blend factor.
 * @param blendDst - destination RGB components blend factor.
 * @param blendSrcAlpha - source alpha component blend factor.
 * @param blendDstAlpha - destination alpha component blend factor.
 *
 * List of possible blend factor values: zero, one, srcColor, oneMinusSrcColor, dstColor, oneMinusDstColor, srcAlpha,
 * oneMinusSrcAlpha, dstAlpha, oneMinusDstAlpha, constantColor, oneMinusConstantColor, constantAlpha, oneMinusConstantAlpha,
 * srcAlphaSaturate.
 */
class BlendingWidget : public virtual widget{
	bool isBlendingEnabled_v = true;
public:
	/**
	 * @brief Structure holding blending settings.
	 */
	struct BlendingParams{
		Renderer::BlendFactor_e src;
		Renderer::BlendFactor_e dst;
		Renderer::BlendFactor_e srcAlpha;
		Renderer::BlendFactor_e dstAlpha;

		bool operator==(const BlendingParams& b){
			return this->src == b.src && this->dst == b.dst && this->srcAlpha == b.srcAlpha && this->dstAlpha == b.dstAlpha;
		}
	};
private:
	BlendingParams blend_v = {
		Renderer::BlendFactor_e::SRC_ALPHA,
		Renderer::BlendFactor_e::ONE_MINUS_SRC_ALPHA,
		Renderer::BlendFactor_e::ONE,
		Renderer::BlendFactor_e::ONE_MINUS_SRC_ALPHA
	};

protected:
	BlendingWidget(const stob::Node* chain);

	BlendingWidget(const puu::trees& desc);

public:
	BlendingWidget(const BlendingWidget&) = delete;
	BlendingWidget& operator=(const BlendingWidget&) = delete;

	/**
	 * @brief Set blending to rendering context.
	 */
	void applyBlending() const;

	/**
	 * @brief Enable/disable blending for this widget.
	 */
	void setBlendingEnabled(bool enable);

	/**
	 * @brief Check if blending is enabled for this widget.
	 */
	bool isBlendingEnabled()const noexcept{
		return this->isBlendingEnabled_v;
	}

	/**
	 * @brief Set blending parameters for this widget.
	 */
	void setBlendingParams(const BlendingParams& blend);

	/**
	 * @brief Get blending parameters of this widget.
	 * @return Blending parameters of this widget.
	 */
	const BlendingParams& blendingParams()const noexcept{
		return this->blend_v;
	}

	/**
	 * @brief Invoked when blending parameters are changed.
	 * Also invoked if blending enabled/disabled status has changed.
	 */
	virtual void onBlendingChanged(){}
};

}
