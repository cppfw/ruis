#include "BlendingWidget.hpp"

#include "../../util/util.hpp"

#include "../../context.hpp"

//TODO: remove
#include "../../gui.hpp"

using namespace morda;



namespace{
const std::map<std::string, renderer::blend_factor> blendFactorNames_c = {
	{"zero", renderer::blend_factor::zero},
	{"one", renderer::blend_factor::one},
	{"srcColor", renderer::blend_factor::src_color},
	{"oneMinusSrcColor", renderer::blend_factor::one_minus_src_color},
	{"dstColor", renderer::blend_factor::dst_color},
	{"oneMinusDstColor", renderer::blend_factor::one_minus_dst_color},
	{"srcAlpha", renderer::blend_factor::src_alpha},
	{"oneMinusSrcAlpha", renderer::blend_factor::one_minus_src_alpha},
	{"dstAlpha", renderer::blend_factor::dst_alpha},
	{"oneMinusDstAlpha", renderer::blend_factor::one_minus_dst_alpha},
	{"constantColor", renderer::blend_factor::constant_color},
	{"oneMinusConstantColor", renderer::blend_factor::one_minus_constant_color},
	{"constantAlpha", renderer::blend_factor::constant_alpha},
	{"oneMinusConstantAlpha", renderer::blend_factor::one_minus_constant_alpha},
	{"srcAlphaSaturate", renderer::blend_factor::src_alpha_saturate}
};

renderer::blend_factor blendFactorFromString(const std::string& s){
	auto i = blendFactorNames_c.find(s);
	if(i == blendFactorNames_c.end()){
		return renderer::blend_factor::one;
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
			this->blend_v.src_alpha = blendFactorFromString(get_property_value(p).to_string());
		}else if(p.value == "blendDstAlpha"){
			this->blend_v.dst_alpha = blendFactorFromString(get_property_value(p).to_string());
		}
	}
}

void BlendingWidget::applyBlending() const{
	auto& r = *this->context->renderer;
	r.set_blend_enabled(this->isBlendingEnabled());
	if(this->isBlendingEnabled()){
		r.set_blend_func(this->blend_v.src, this->blend_v.dst, this->blend_v.src_alpha, this->blend_v.dst_alpha);
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
