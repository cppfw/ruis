#pragma once

#include "core/Widget.hpp"

namespace morda{

class BlendWidget : public virtual Widget{
	bool isBlendEnabled_v;
public:
	struct Blend{
		Render::BlendFactor_e src;
		Render::BlendFactor_e dst;
		Render::BlendFactor_e srcAlpha;
		Render::BlendFactor_e dstAlpha;
		
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
	
	void applyBlending() const;

	void setBlendEnabled(bool enable);
	
	bool isBlendEnabled()const noexcept{
		return this->isBlendEnabled_v;
	}
	
	void setBlend(const Blend& blend);
	
	const Blend& blend()const noexcept{
		return this->blend_v;
	}
	
	virtual void onBlendChanged(){}
	
	
private:

};

}
