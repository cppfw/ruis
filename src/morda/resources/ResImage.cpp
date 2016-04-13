#include <memory>

#include <svgren/render.hpp>

//Some bad stuff defines OVERFLOW macro and there is an enum value with same name in svgdom/dom.h.
#ifdef OVERFLOW
#	undef OVERFLOW
#endif

#include <svgdom/dom.hpp>

#include "ResImage.hpp"

#include "../App.hpp"
#include "../util/util.hpp"
#include "../shaders/PosTexShader.hpp"


using namespace morda;



ResRasterImage::ResRasterImage(std::shared_ptr<ResTexture> tex, const Rectr& rect) :
		tex(tex),
		dim_var(rect.d.abs())
{
	this->texCoords[3] = Vec2r(rect.left(), this->tex->tex().dim().y - rect.bottom()).compDivBy(this->tex->tex().dim());
	this->texCoords[2] = Vec2r(rect.right(), this->tex->tex().dim().y - rect.bottom()).compDivBy(this->tex->tex().dim());
	this->texCoords[1] = Vec2r(rect.right(), this->tex->tex().dim().y - rect.top()).compDivBy(this->tex->tex().dim());
	this->texCoords[0] = Vec2r(rect.left(), this->tex->tex().dim().y - rect.top()).compDivBy(this->tex->tex().dim());
}



std::shared_ptr<ResRasterImage> ResRasterImage::load(const stob::Node& chain, const papki::File& fi){
	auto tex = App::inst().resMan.load<ResTexture>(chain.side("tex").up().value());
	
	Rectr rect;
	if(auto n = chain.childOfThisOrNext("rect")){
		rect = makeRectrFromSTOB(n);
	}else{
		rect = Rectr(Vec2r(0, 0), tex->tex().dim());
	}
	
	return utki::makeShared<ResRasterImage>(tex, rect);
}



void ResRasterImage::render(const Matr4r& matrix, PosTexShader& s) const{
	this->tex->tex().bind();
	
	kolme::Matr4f matr(matrix);
	matr.scale(this->dim_var);
	
	s.SetMatrix(matr);
	s.render(utki::wrapBuf(PosShader::quad01Fan), utki::wrapBuf(this->texCoords));
}

std::shared_ptr<ResImage> ResImage::load(const stob::Node& chain, const papki::File& fi) {
	if(auto f = chain.thisOrNext("file").node()){
		if(auto fn = f->child()){
			fi.setPath(fn->value());
			if(fi.ext().compare("svg") == 0){
				return ResSvgImage::load(chain, fi);
			}
		}
	}
	
	return ResRasterImage::load(chain, fi);
}


namespace{
class ResSvgImageInternal : public ResSvgImage{
	std::unique_ptr<svgdom::SvgElement> dom;
public:
	ResSvgImageInternal(decltype(dom) dom) :
			dom(std::move(dom))
	{}
	
	Vec2r dim(real dpi)const noexcept override{
		return Vec2r(
				this->dom->width.toPx(dpi),
				this->dom->height.toPx(dpi)
			);
	}
	
	class SvgImage : public Image{
		std::weak_ptr<const ResSvgImageInternal> parent;
	public:
		SvgImage(std::shared_ptr<const ResSvgImageInternal> parent, Texture2D&& tex) :
				Image(std::move(tex)),
				parent(parent)
		{}

		~SvgImage()noexcept{
			if(auto p = this->parent.lock()){
				p->cache.erase(std::make_tuple(this->tex.dim().x, this->tex.dim().y));
			}
		}
	};
	
	std::shared_ptr<Image> get(Vec2r forDim)const override{
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
		ASSERT(imWidth * imHeight == pixels.size())
		
		
		auto img = utki::makeShared<SvgImage>(this->sharedFromThis(this), Texture2D(imWidth, pixels));

		this->cache[std::make_tuple(imWidth, imHeight)] = img;

		return img;
	}
	
	mutable std::map<std::tuple<unsigned, unsigned>, std::weak_ptr<Image>> cache;
};
}

std::shared_ptr<ResSvgImage> ResSvgImage::load(const stob::Node& chain, const papki::File& fi) {
	fi.setPath(chain.side("file").up().value());
	
	return utki::makeShared<ResSvgImageInternal>(svgdom::load(fi));
}


void ResImage::Image::render(const Matr4r& matrix, PosTexShader& s) const {
	this->tex.bind();
	
	kolme::Matr4f matr(matrix);
	matr.scale(this->tex.dim());
	
	s.SetMatrix(matr);
	s.render(utki::wrapBuf(PosShader::quad01Fan), utki::wrapBuf(PosTexShader::quadFanTexCoords));
}

