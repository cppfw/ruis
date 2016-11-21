#include "util.hpp"

#include <utki/debug.hpp>
#include <utki/config.hpp>

#include "../Morda.hpp"

using namespace morda;



morda::Vec2r morda::makeVec2rFromSTOB(const stob::Node* chain){
	unsigned i;
	morda::Vec2r ret;
	
	float v = 0;
	for(i = 0; i != 2 && chain; ++i, chain = chain->next()){
		v = chain->asFloat();
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}


Rectr morda::makeRectrFromSTOB(const stob::Node* chain){
	Vec2r p = makeVec2rFromSTOB(chain);
	for(unsigned i = 0; i != 2 && chain; ++i, chain = chain->next()){}
	Vec2r d = makeVec2rFromSTOB(chain);
	return Rectr(p, d);
}

Sidesr morda::makeSidesrFromSTOB(const stob::Node* chain){
	Vec2r p = makeVec2rFromSTOB(chain);
	for(unsigned i = 0; i != 2 && chain; ++i, chain = chain->next()){}
	Vec2r d = makeVec2rFromSTOB(chain);
	return Sidesr(p.x, p.y, d.x, d.y);
}


morda::Vec2r morda::dimVec2rFromSTOB(const stob::Node* chain){
	unsigned i;
	morda::Vec2r ret;
	
	float v = 0;
	for(i = 0; i != 2 && chain; ++i, chain = chain->next()){
		v = dimValueFromSTOB(*chain);
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}



real morda::dimValueFromSTOB(const stob::Node& n){
	//check if millimeters
	if(n.length() >= 2 && n.value()[n.length() - 1] == 'm' && n.value()[n.length() - 2] == 'm'){
		return Morda::inst().units.mmToPx(n.asFloat());
	}else if(n.length() >= 2 && n.value()[n.length() - 1] == 't' && n.value()[n.length() - 2] == 'p'){ //check if in points
		return Morda::inst().units.ptToPx(n.asFloat());
	}
	
	return n.asFloat();
}



std::tuple<std::unique_ptr<stob::Node>, stob::Node*> morda::resolveIncludes(papki::File& fi, std::unique_ptr<stob::Node> begin){
	if(!begin){
		return std::make_tuple(nullptr, nullptr);
	}
	
	const char* DIncludeTag = "include";
	
	auto n = begin->thisOrNext(DIncludeTag);
	for(; n.node();){
		ASSERT(n.node())
		stob::Node* incPathNode = n.node()->child();
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
				lastChild->insertNext(n.node()->chopNext());
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
			n = n.node()->next(DIncludeTag);
		}
	}
	return std::make_tuple(std::move(begin), n.prev());
}



kolme::Vec2b morda::makeVec2bFromSTOB(const stob::Node* chain){
	unsigned i;
	kolme::Vec2b ret;
	
	bool v = false;
	for(i = 0; i != 2 && chain; ++i, chain = chain->next()){
		v = chain->asBool();
		ret[i] = v;
	}
	
	for(; i != 2; ++i){
		ret[i] = v;
	}
	
	return ret;
}



real morda::findDotsPerPt(kolme::Vec2ui resolution, kolme::Vec2ui screenSizeMm){
	
	//NOTE: for ordinary desktop displays the PT size should be equal to 1 pixel.
	//For high density displays it should be more than one pixel, depending on display ppi.
	//For handheld devices the size of PT should be determined from physical screen size and pixel resolution.
	
#if M_OS_NAME == M_OS_NAME_ANDROID
	return real(1);//TODO:
#elif M_OS_NAME == M_OS_NAME_IOS
	return real(1);//TODO:
#else
	return real(1);
#endif
}



float morda::dimValueFromLayoutStob(const stob::Node& n){
	if(n == "min"){
		return Widget::LayoutParams::min_c;
	}else if(n == "max"){
		return Widget::LayoutParams::max_c;
	}else if(n == "fill"){
		return Widget::LayoutParams::fill_c;
	}
	return dimValueFromSTOB(n);
}



const stob::Node* morda::getProperty(const stob::Node* chain, const char* property){
	if(!chain){
		return nullptr;
	}
	if(auto n = chain->thisOrNext(property).node()){
		return n->child();
	}
	return nullptr;
}


//TODO: remove
Texture2D morda::loadTexture(const papki::File& fi){
	Image image(fi);
//	TRACE(<< "ResTexture::Load(): image loaded" << std::endl)
	image.flipVertical();
	return Texture2D(image);
}


morda::Texture2D_n::TexType_e morda::numChannelsToTexType(unsigned numChannels){
	switch(numChannels){
		default:
			ASSERT(false)
		case 1:
			return morda::Texture2D_n::TexType_e::GREY;
			break;
		case 2:
			return morda::Texture2D_n::TexType_e::GREYA;
			break;
		case 3:
			return morda::Texture2D_n::TexType_e::RGB;
			break;
		case 4:
			return morda::Texture2D_n::TexType_e::RGBA;
			break;
	}
}

std::shared_ptr<Texture2D_n> morda::loadTexture_n(const papki::File& fi){
	Image image(fi);
//	TRACE(<< "ResTexture::Load(): image loaded" << std::endl)
	image.flipVertical();	
	
	return morda::inst().renderer().factory->createTexture2D(
			numChannelsToTexType(image.numChannels()),
			image.dim(),
			image.buf()
		);
}


void morda::applySimpleAlphaBlending(){
	Render::setBlendEnabled(true);
	Render::setBlendFunc(Render::BlendFactor_e::SRC_ALPHA, Render::BlendFactor_e::ONE_MINUS_SRC_ALPHA, Render::BlendFactor_e::ONE, Render::BlendFactor_e::ONE_MINUS_SRC_ALPHA);
}


kolme::Vec4f morda::colorToVec4f(std::uint32_t color){
	return kolme::Vec4f(
			float(color & 0xff) / float(0xff),
			float((color >> 8) & 0xff) / float(0xff),
			float((color >> 16) & 0xff) / float(0xff),
			float((color >> 24) & 0xff) / float(0xff)
		);
}
