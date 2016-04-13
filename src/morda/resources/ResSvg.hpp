/* 
 * File:   ResSvg.hpp
 * Author: ivan
 *
 * Created on April 13, 2016, 2:10 PM
 */

#pragma once

#include "../ResourceManager.hpp"

#include "../render/Texture2D.hpp"

#include "../shaders/PosTexShader.hpp"


//Some bad stuff defines OVERFLOW macro and tehre is an enum vaslue with same name in svgdom/dom.h.
#ifdef OVERFLOW
#	undef OVERFLOW
#endif

#include <svgdom/dom.hpp>

namespace morda{

class ResSvg : public Resource{
	friend class ResourceManager;
	
	std::unique_ptr<svgdom::SvgElement> dom;
public:
	ResSvg(decltype(dom) dom) :
			dom(std::move(dom))
	{}
	
	ResSvg(const ResSvg&) = delete;
	ResSvg& operator=(const ResSvg&) = delete;
	
	
	class Image : public utki::Shared{
		Texture2D tex;
	public:
		Image(Texture2D&& tex) :
				tex(std::move(tex))
		{}

		void render(const Matr4r& matrix, PosTexShader& s)const;
	};
	
	Vec2r dim(real dotsPerInch)const noexcept;
	
	/**
	 * @brief Get raster image of given dimensions.
	 * @param forDim - dimensions request for raster image.
	 *        If any of the dimensions is 0 then it will be adjusted to preserve aspect ratio.
	 */
	std::shared_ptr<Image> get(Vec2r forDim)const;
	
private:
	mutable std::map<std::tuple<unsigned, unsigned>, std::weak_ptr<Image>> cache;
	
	static std::shared_ptr<ResSvg> load(const stob::Node& chain, const papki::File& fi);
};

}
