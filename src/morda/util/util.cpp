#include "util.hpp"

#include <utki/debug.hpp>
#include <utki/config.hpp>

#include "../Morda.hpp"

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

real morda::parse_dimension_value(const puu::leaf& l){
	//check if millimeters
	if(l.length() >= 2 && l[l.length() - 1] == 'm' && l[l.length() - 2] == 'm'){
		return Morda::inst().units.mmToPx(l.to_float());
	}else if(l.length() >= 2 && l[l.length() - 1] == 'p' && l[l.length() - 2] == 'd'){ //check if in density pixels
		return Morda::inst().units.dpToPx(l.to_float());
	}

	return l.to_float();
}



std::tuple<std::unique_ptr<stob::Node>, stob::Node*> morda::resolveIncludes(const papki::File& fi, std::unique_ptr<stob::Node> begin){
	if(!begin){
		return std::make_tuple(nullptr, nullptr);
	}

	const char* DIncludeTag = "include";

	auto n = begin->thisOrNext(DIncludeTag);
	for(; n.get_node();){
		ASSERT(n.get_node())
		stob::Node* incPathNode = n.get_node()->child();
		if(!incPathNode){
			throw Exc("include tag without value encountered in resource script");
		}
		TRACE(<< "ResolveIncludes(): incPathNode->Value = " << incPathNode->value() << std::endl)

		fi.setPath(fi.dir() + incPathNode->value());
		std::unique_ptr<stob::Node> incNodes = stob::load(fi);

		//recursive call
		auto ri = resolveIncludes(fi, std::move(incNodes));

		stob::Node* lastChild = std::get<1>(ri);

		if(lastChild){
			//substitute includes
			if(!n.prev()){
				//include tag is the very first tag

				ASSERT(!lastChild->next())
				lastChild->insertNext(n.get_node()->chopNext());
				begin = std::move(std::get<0>(ri));
			}else{
				//include tag is not the first one

				n.prev()->removeNext();

				ASSERT(!lastChild->next())
				std::unique_ptr<stob::Node> tail = n.prev()->chopNext();
				n.prev()->setNext(std::move(std::get<0>(ri)));
				lastChild->setNext(std::move(tail));
			}
			n = lastChild->next(DIncludeTag);
		}else{
			n = n.get_node()->next(DIncludeTag);
		}
	}
	return std::make_tuple(std::move(begin), n.prev());
}


real morda::parse_layout_dimension_value(const puu::leaf& l){
	if(l == "min"){
		return widget::layout_params::min;
	}else if(l == "fill"){
		return widget::layout_params::fill;
	}else if(l == "max"){
		return widget::layout_params::max;
	}
	return parse_dimension_value(l);
}


morda::Texture2D::TexType_e morda::numChannelsToTexType(unsigned numChannels){
	switch(numChannels){
		default:
			ASSERT(false)
		case 1:
			return morda::Texture2D::TexType_e::GREY;
			break;
		case 2:
			return morda::Texture2D::TexType_e::GREYA;
			break;
		case 3:
			return morda::Texture2D::TexType_e::RGB;
			break;
		case 4:
			return morda::Texture2D::TexType_e::RGBA;
			break;
	}
}

std::shared_ptr<Texture2D> morda::loadTexture(const papki::File& fi){
	RasterImage image(fi);
//	TRACE(<< "ResTexture::Load(): image loaded" << std::endl)

	return morda::inst().renderer().factory->createTexture2D(
			numChannelsToTexType(image.numChannels()),
			image.dim(),
			image.buf()
		);
}


void morda::applySimpleAlphaBlending(){
	morda::inst().renderer().setBlendEnabled(true);
	morda::inst().renderer().setBlendFunc(
			Renderer::BlendFactor_e::SRC_ALPHA,
			Renderer::BlendFactor_e::ONE_MINUS_SRC_ALPHA,
			Renderer::BlendFactor_e::ONE,
			Renderer::BlendFactor_e::ONE_MINUS_SRC_ALPHA
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
	return l.length() == 0 || l[0] < 'A' || 'Z' < l[0];
}

bool morda::is_property(const puu::tree& t){
	return is_leaf_property(t.value) && t.children.size() != 0;
}
