#include "util.hpp"

#include <utki/debug.hpp>
#include <utki/config.hpp>

#include "../context.hpp"

#include "RasterImage.hpp"

using namespace morda;


morda::Vec2r morda::parse_vec2(puu::forest::const_iterator begin, puu::forest::const_iterator end){
	morda::Vec2r ret;

	unsigned n = 0;
	real v = 0;
	for(auto i = begin; n != 2 && i != end; ++n, ++i){
		v = real(i->value.to_float());
		ret[n] = v;
	}

	for(; n != 2; ++n){
		ret[n] = v;
	}

	return ret;
}

morda::Rectr morda::parse_rect(const puu::forest& desc){
	Vec2r p = parse_vec2(desc.begin(), desc.end());
	Vec2r d = parse_vec2(std::next(desc.begin(), std::min(size_t(2), desc.size())), desc.end());
	return Rectr(p, d);
}

morda::Sidesr morda::parse_sides(const puu::forest& desc){
	Vec2r p = parse_vec2(desc.begin(), desc.end());
	Vec2r d = parse_vec2(std::next(desc.begin(), std::min(size_t(2), desc.size())), desc.end());
	return Sidesr(p.x, p.y, d.x, d.y);
}

real morda::parse_dimension_value(const puu::leaf& l, const morda::units& units){
	// check if millimeters
	if(l.length() >= 2 && l[l.length() - 1] == 'm' && l[l.length() - 2] == 'm'){
		return units.mm_to_px(l.to_float());
	}else if(l.length() >= 2 && l[l.length() - 1] == 'p' && l[l.length() - 2] == 'd'){ //check if in density pixels
		return units.dp_to_px(l.to_float());
	}

	if(l.empty()){
		return real(0);
	}

	return real(l.to_float());
}


real morda::parse_layout_dimension_value(const puu::leaf& l, const morda::units& units){
	if(l == "min"){
		return widget::layout_params::min;
	}else if(l == "fill"){
		return widget::layout_params::fill;
	}else if(l == "max"){
		return widget::layout_params::max;
	}
	return parse_dimension_value(l, units);
}


morda::texture_2d::type morda::num_channels_to_texture_type(unsigned numChannels){
	switch(numChannels){
		default:
			ASSERT(false)
		case 1:
			return morda::texture_2d::type::grey;
			break;
		case 2:
			return morda::texture_2d::type::grey_alpha;
			break;
		case 3:
			return morda::texture_2d::type::rgb;
			break;
		case 4:
			return morda::texture_2d::type::rgba;
			break;
	}
}

std::shared_ptr<texture_2d> morda::load_texture(renderer& r, const papki::file& fi){
	RasterImage image(fi);
//	TRACE(<< "ResTexture::Load(): image loaded" << std::endl)

	return r.factory->create_texture_2d(
			num_channels_to_texture_type(image.numChannels()),
			image.dim(),
			image.buf()
		);
}


void morda::set_simple_alpha_blending(renderer& r){
	r.set_blend_enabled(true);
	r.set_blend_func(
			renderer::blend_factor::src_alpha,
			renderer::blend_factor::one_minus_src_alpha,
			renderer::blend_factor::one,
			renderer::blend_factor::one_minus_src_alpha
		);
}


r4::vec4f morda::colorToVec4f(std::uint32_t color){
	return r4::vec4f(
			float(color & 0xff) / float(0xff),
			float((color >> 8) & 0xff) / float(0xff),
			float((color >> 16) & 0xff) / float(0xff),
			float((color >> 24) & 0xff) / float(0xff)
		);
}

bool morda::is_leaf_property(const puu::leaf& l){
	return l.empty() || l[0] != '@';
}

bool morda::is_property(const puu::tree& t){
	return is_leaf_property(t.value) && t.children.size() != 0;
}
