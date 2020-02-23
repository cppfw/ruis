#include "BlendingWidget.hpp"

#include "../../util/util.hpp"

#include "../../context.hpp"

//TODO: remove
#include "../../gui.hpp"

using namespace morda;



namespace{
const std::map<std::string, renderer::BlendFactor_e> blendFactorNames_c = {
	{"zero", renderer::BlendFactor_e::ZERO},
	{"one", renderer::BlendFactor_e::ONE},
	{"srcColor", renderer::BlendFactor_e::SRC_COLOR},
	{"oneMinusSrcColor", renderer::BlendFactor_e::ONE_MINUS_SRC_COLOR},
	{"dstColor", renderer::BlendFactor_e::DST_COLOR},
	{"oneMinusDstColor", renderer::BlendFactor_e::ONE_MINUS_DST_COLOR},
	{"srcAlpha", renderer::BlendFactor_e::SRC_ALPHA},
	{"oneMinusSrcAlpha", renderer::BlendFactor_e::ONE_MINUS_SRC_ALPHA},
	{"dstAlpha", renderer::BlendFactor_e::DST_ALPHA},
	{"oneMinusDstAlpha", renderer::BlendFactor_e::ONE_MINUS_DST_ALPHA},
	{"constantColor", renderer::BlendFactor_e::CONSTANT_COLOR},
	{"oneMinusConstantColor", renderer::BlendFactor_e::ONE_MINUS_CONSTANT_COLOR},
	{"constantAlpha", renderer::BlendFactor_e::CONSTANT_ALPHA},
	{"oneMinusConstantAlpha", renderer::BlendFactor_e::ONE_MINUS_CONSTANT_ALPHA},
	{"srcAlphaSaturate", renderer::BlendFactor_e::SRC_ALPHA_SATURATE}
};

renderer::BlendFactor_e blendFactorFromString(const std::string& s){
	auto i = blendFactorNames_c.find(s);
	if(i == blendFactorNames_c.end()){
		return renderer::BlendFactor_e::ONE;
	}
	return i->second;
}
}


BlendingWidget::BlendingWidget(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
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
	auto& r = *this->context->renderer;
	r.setBlendEnabled(this->isBlendingEnabled());
	if(this->isBlendingEnabled()){
		r.setBlendFunc(this->blend_v.src, this->blend_v.dst, this->blend_v.srcAlpha, this->blend_v.dstAlpha);
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
