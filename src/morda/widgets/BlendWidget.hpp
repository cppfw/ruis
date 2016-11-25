#pragma once

#include "core/Widget.hpp"

#include "../render/Renderer.hpp"

namespace morda{

/**
 * @brief Basic widget which allows configuring alpha blending when rendering.
 * @param blend - enable (true) or disabple (false) alpha blending.
 * @param blendSrc - source RGB components blend factor.
 * @param blendDst - destination RGB components blend factor.
 * @param blendSrcAlpha - source alpha component blend factor.
 * @param blendDstAlpha - destination alpha component blend factor.
 * 
 * List of possible blend factor values: zero, one, srcColor, oneMinusSrcColor, dstColor, oneMinusDstColor, srcAlpha,
 * oneMinusSrcAlpha, dstAlpha, oneMinusDstAlpha, constantColor, oneMinusConstantColor, constantAlpha, oneMinusConstantAlpha,
 * srcAlphaSaturate.
 */
class BlendWidget : public virtual Widget{
	bool isBlendEnabled_v;
public:
	/**
	 * @brief Structure holding blending settings.
	 */
	struct Blend{
		Renderer::BlendFactor_e src;
		Renderer::BlendFactor_e dst;
		Renderer::BlendFactor_e srcAlpha;
		Renderer::BlendFactor_e dstAlpha;
		
		bool operator==(const Blend& b){
			return this->src == b.src && this->dst == b.dst && this->srcAlpha == b.srcAlpha && this->dstAlpha == b.dstAlpha;
		}
	};
private:
	Blend blend_v;
	
protected:
	BlendWidget(const stob::Node* chain);
	
public:
	BlendWidget(const BlendWidget&) = delete;
	BlendWidget& operator=(const BlendWidget&) = delete;
	
	/**
	 * @brief Set blending to rendering context.
	 */
	void applyBlending() const;

	/**
	 * @brief Enable/disable blending for this widget.
	 */
	void setBlendEnabled(bool enable);
	
	/**
	 * @brief Checi if blending is enabled for this widget.
	 */
	bool isBlendEnabled()const noexcept{
		return this->isBlendEnabled_v;
	}
	
	/**
	 * @brief Set blending parameters for this widget.
	 */
	void setBlend(const Blend& blend);
	
	/**
	 * @brief Get blending parameters of this widget.
	 * @return Blending parameters of this widget.
	 */
	const Blend& blend()const noexcept{
		return this->blend_v;
	}
	
	/**
	 * @brief Invoked when blening parameters are changed.
	 * Also invoked if blending enabled/disabled status has changed.
	 */
	virtual void onBlendChanged(){}
};

}
