#include "BlendingWidget.hpp"

#include "../../util/util.hpp"

#include "../../context.hpp"

using namespace morda;



namespace{
const std::map<std::string, Renderer::BlendFactor_e> blendFactorNames_c = {
	{"zero", Renderer::BlendFactor_e::ZERO},
	{"one", Renderer::BlendFactor_e::ONE},
	{"srcColor", Renderer::BlendFactor_e::SRC_COLOR},
	{"oneMinusSrcColor", Renderer::BlendFactor_e::ONE_MINUS_SRC_COLOR},
	{"dstColor", Renderer::BlendFactor_e::DST_COLOR},
	{"oneMinusDstColor", Renderer::BlendFactor_e::ONE_MINUS_DST_COLOR},
	{"srcAlpha", Renderer::BlendFactor_e::SRC_ALPHA},
	{"oneMinusSrcAlpha", Renderer::BlendFactor_e::ONE_MINUS_SRC_ALPHA},
	{"dstAlpha", Renderer::BlendFactor_e::DST_ALPHA},
	{"oneMinusDstAlpha", Renderer::BlendFactor_e::ONE_MINUS_DST_ALPHA},
	{"constantColor", Renderer::BlendFactor_e::CONSTANT_COLOR},
	{"oneMinusConstantColor", Renderer::BlendFactor_e::ONE_MINUS_CONSTANT_COLOR},
	{"constantAlpha", Renderer::BlendFactor_e::CONSTANT_ALPHA},
	{"oneMinusConstantAlpha", Renderer::BlendFactor_e::ONE_MINUS_CONSTANT_ALPHA},
	{"srcAlphaSaturate", Renderer::BlendFactor_e::SRC_ALPHA_SATURATE}
};

Renderer::BlendFactor_e blendFactorFromString(const std::string& s){
	auto i = blendFactorNames_c.find(s);
	if(i == blendFactorNames_c.end()){
		return Renderer::BlendFactor_e::ONE;
	}
	return i->second;
}
}


BlendingWidget::BlendingWidget(const puu::forest& desc) :
		widget(desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "blend"){
			this->isBlendingEnabled_v = get_property_value(p).to_bool();
		}else if(p.value == "blendSrc"){
			this->blend_v.src = blendFactorFromString(get_property_value(p).to_string());
		}else if(p.value == "blendDst"){
			this->blend_v.dst = blendFactorFromString(get_property_value(p).to_string());
		}else if(p.value == "blendSrcAlpha"){
			this->blend_v.srcAlpha = blendFactorFromString(get_property_value(p).to_string());
		}else if(p.value == "blendDstAlpha"){
			this->blend_v.dstAlpha = blendFactorFromString(get_property_value(p).to_string());
		}
	}
}

void BlendingWidget::applyBlending() const{
	morda::inst().renderer->setBlendEnabled(this->isBlendingEnabled());
	if(this->isBlendingEnabled()){
		morda::inst().renderer->setBlendFunc(this->blend_v.src, this->blend_v.dst, this->blend_v.srcAlpha, this->blend_v.dstAlpha);
	}
}

void BlendingWidget::setBlendingEnabled(bool enable){
	if(this->isBlendingEnabled_v == enable){
		return;
	}
	this->isBlendingEnabled_v = enable;
	this->onBlendingChanged();
}

void BlendingWidget::setBlendingParams(const BlendingParams& blend){
	if(this->blend_v == blend){
		return;
	}
	this->blend_v = blend;
	this->onBlendingChanged();
}
