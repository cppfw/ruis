#include "BlendWidget.hpp"

#include "../util/util.hpp"

using namespace morda;



namespace{
std::map<std::string, Render::BlendFactor_e> blendFactorNames_d = {
	{"zero", Render::BlendFactor_e::ZERO},
	{"one", Render::BlendFactor_e::ONE},
	{"srcColor", Render::BlendFactor_e::SRC_COLOR},
	{"oneMinusSrcColor", Render::BlendFactor_e::ONE_MINUS_SRC_COLOR},
	{"dstColor", Render::BlendFactor_e::DST_COLOR},
	{"oneMinusDstColor", Render::BlendFactor_e::ONE_MINUS_DST_COLOR},
	{"srcAlpha", Render::BlendFactor_e::SRC_ALPHA},
	{"oneMinusSrcAlpha", Render::BlendFactor_e::ONE_MINUS_SRC_ALPHA},
	{"dstAlpha", Render::BlendFactor_e::DST_ALPHA},
	{"oneMinusDstAlpha", Render::BlendFactor_e::ONE_MINUS_DST_ALPHA},
	{"constantColor", Render::BlendFactor_e::CONSTANT_COLOR},
	{"oneMinusConstantColor", Render::BlendFactor_e::ONE_MINUS_CONSTANT_COLOR},
	{"constantAlpha", Render::BlendFactor_e::CONSTANT_ALPHA},
	{"oneMinusConstantAlpha", Render::BlendFactor_e::ONE_MINUS_CONSTANT_ALPHA},
	{"srcAlphaSaturate", Render::BlendFactor_e::SRC_ALPHA_SATURATE}
};

Render::BlendFactor_e blendFactorFromString(const std::string& s){
	auto i = blendFactorNames_d.find(s);
	if(i == blendFactorNames_d.end()){
		return Render::BlendFactor_e::ONE;
	}
	return i->second;
}
}



BlendWidget::BlendWidget(const stob::Node* chain) :
		Widget(chain)
{
	if(auto n = getProperty(chain, "blend")){
		this->isBlendEnabled_v = n->asBool();
	}else{
		this->isBlendEnabled_v = true;
	}
	
	if(auto n = getProperty(chain, "blendSrc")){
		this->blend_v.src = blendFactorFromString(n->value());
	}else{
		this->blend_v.src = Render::BlendFactor_e::SRC_ALPHA;
	}
	
	if(auto n = getProperty(chain, "blendDst")){
		this->blend_v.dst = blendFactorFromString(n->value());
	}else{
		this->blend_v.dst = Render::BlendFactor_e::ONE_MINUS_SRC_ALPHA;
	}
	
	if(auto n = getProperty(chain, "blendSrcAlpha")){
		this->blend_v.srcAlpha = blendFactorFromString(n->value());
	}else{
		this->blend_v.srcAlpha = Render::BlendFactor_e::ONE;
	}
	
	if(auto n = getProperty(chain, "blendDstAlpha")){
		this->blend_v.dstAlpha = blendFactorFromString(n->value());
	}else{
		this->blend_v.dstAlpha = Render::BlendFactor_e::ONE_MINUS_SRC_ALPHA;
	}
}

void BlendWidget::applyBlending() const{
	Render::setBlendEnabled(this->isBlendEnabled());
	if(this->isBlendEnabled()){
		Render::setBlendFunc(this->blend_v.src, this->blend_v.dst, this->blend_v.srcAlpha, this->blend_v.dstAlpha);
	}
}

void BlendWidget::setBlendEnabled(bool enable){
	if(this->isBlendEnabled_v == enable){
		return;
	}
	this->isBlendEnabled_v = enable;
	this->onBlendChanged();
}

void BlendWidget::setBlend(const Blend& blend){
	if(this->blend_v == blend){
		return;
	}
	this->blend_v = blend;
	this->onBlendChanged();
}
