/* 
 * File:   ResSvg.cpp
 * Author: ivan
 * 
 * Created on April 13, 2016, 2:10 PM
 */

#include "ResSvg.hpp"

#include <svgren/render.hpp>

using namespace morda;



std::shared_ptr<ResSvg> ResSvg::load(const stob::Node& chain, const papki::File& fi) {
	fi.setPath(chain.side("file").up().value());
	
	return utki::makeShared<ResSvg>(svgdom::load(fi));
}

Vec2r ResSvg::dim(real dotsPerInch) const noexcept{
	return Vec2r(
			this->dom->width.toPx(dotsPerInch),
			this->dom->height.toPx(dotsPerInch)
		);
}

std::shared_ptr<ResSvg::Image> ResSvg::get(Vec2r forDim) const {
	unsigned imWidth = unsigned(forDim.x);
	unsigned imHeight = unsigned(forDim.y);
	
	{//check if in cache
		auto i = this->cache.find(std::make_tuple(imWidth, imHeight));
		if(i != this->cache.end()){
			if(auto p = i->second.lock()){
				return p;
			}
		}
	}
	
	auto pixels = svgren::render(*this->dom, imWidth, imHeight);
	
	auto img = utki::makeShared<Image>(Texture2D(imWidth, pixels));
	
	this->cache[std::make_tuple(imWidth, imHeight)] = img;
	
	return img;
}
