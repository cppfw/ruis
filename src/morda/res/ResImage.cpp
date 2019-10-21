#include <memory>

#include <svgren/render.hpp>

//Some bad stuff defines OVERFLOW macro and there is an enum value with same name in svgdom/dom.h.
#ifdef OVERFLOW
#	undef OVERFLOW
#endif

#include <svgdom/dom.hpp>

#include "ResImage.hpp"

#include "../Morda.hpp"

#include "../util/util.hpp"



using namespace morda;



ResAtlasImage::ResAtlasImage(std::shared_ptr<ResTexture> tex, const Rectr& rect) :
		ResImage::QuadTexture(rect.d.abs()),
		tex(std::move(tex))
{
//	this->texCoords[3] = Vec2r(rect.left(), this->tex->tex().dim().y - rect.bottom()).compDivBy(this->tex->tex().dim());
//	this->texCoords[2] = Vec2r(rect.right(), this->tex->tex().dim().y - rect.bottom()).compDivBy(this->tex->tex().dim());
//	this->texCoords[1] = Vec2r(rect.right(), this->tex->tex().dim().y - rect.top()).compDivBy(this->tex->tex().dim());
//	this->texCoords[0] = Vec2r(rect.left(), this->tex->tex().dim().y - rect.top()).compDivBy(this->tex->tex().dim());
	//TODO:
	ASSERT(false)
}

ResAtlasImage::ResAtlasImage(std::shared_ptr<ResTexture> tex) :
		ResImage::QuadTexture(tex->tex().dim()),
		tex(std::move(tex)),
		vao(morda::inst().renderer().posTexQuad01VAO)
{
}



std::shared_ptr<ResAtlasImage> ResAtlasImage::load(const stob::Node& chain, const papki::File& fi){
	auto tex = Morda::inst().resMan.load<ResTexture>(chain.side("tex").up().value());
	
	Rectr rect;
	if(auto n = chain.childOfThisOrNext("rect")){
		rect = makeRectrFromSTOB(n);
		return std::make_shared<ResAtlasImage>(tex, rect);
	}else{
		return std::make_shared<ResAtlasImage>(tex);
	}
}


void ResAtlasImage::render(const Matr4r& matrix, const VertexArray& vao) const {
	morda::inst().renderer().shader->posTex->render(matrix, *this->vao, this->tex->tex());
}



namespace{

class TexQuadTexture : public ResImage::QuadTexture{
protected:
	std::shared_ptr<Texture2D> tex_v;
	
	TexQuadTexture(std::shared_ptr<Texture2D> tex) :
			ResImage::QuadTexture(tex->dim()),
			tex_v(std::move(tex))
	{}
	
public:
	void render(const Matr4r& matrix, const VertexArray& vao) const override{
		morda::inst().renderer().shader->posTex->render(matrix, vao, *this->tex_v);
	}
};
	
class ResRasterImage : public ResImage, public TexQuadTexture{
public:
	ResRasterImage(std::shared_ptr<Texture2D> tex) :
			TexQuadTexture(std::move(tex))
	{}
	
	std::shared_ptr<const ResImage::QuadTexture> get(Vec2r forDim) const override{
		return this->sharedFromThis(this);
	}
	
	Vec2r dim(real dpi) const noexcept override{
		return this->tex_v->dim();
	}
	
	static std::shared_ptr<ResRasterImage> load(const papki::File& fi){
		return std::make_shared<ResRasterImage>(loadTexture(fi));
	}
};

class ResSvgImage : public ResImage{
	std::unique_ptr<svgdom::SvgElement> dom;
public:
	ResSvgImage(decltype(dom) dom) :
			dom(std::move(dom))
	{}
	
	Vec2r dim(real dpi)const noexcept override{
		auto wh = this->dom->getDimensions(dpi);
		return Vec2r(wh[0], wh[1]);
	}
	
	class SvgTexture : public TexQuadTexture{
		std::weak_ptr<const ResSvgImage> parent;
	public:
		SvgTexture(std::shared_ptr<const ResSvgImage> parent, std::shared_ptr<Texture2D> tex) :
				TexQuadTexture(std::move(tex)),
				parent(parent)
		{}

		~SvgTexture()noexcept{
			if(auto p = this->parent.lock()){
				kolme::Vec2ui d = this->tex_v->dim().to<unsigned>();
				p->cache.erase(std::make_tuple(d.x, d.y));
			}
		}
	};
	
	std::shared_ptr<const QuadTexture> get(Vec2r forDim)const override{
//		TRACE(<< "forDim = " << forDim << std::endl)
		unsigned width = unsigned(forDim.x);
//		TRACE(<< "imWidth = " << imWidth << std::endl)
		unsigned height = unsigned(forDim.y);
//		TRACE(<< "imHeight = " << imHeight << std::endl)

		{//check if in cache
			auto i = this->cache.find(std::make_tuple(width, height));
			if(i != this->cache.end()){
				if(auto p = i->second.lock()){
					return p;
				}
			}
		}
//		TRACE(<< "not in cache" << std::endl)

		ASSERT(this->dom)
//		TRACE(<< "width = " << this->dom->width << std::endl)
//		TRACE(<< "height = " << this->dom->height << std::endl)
//		TRACE(<< "dpi = " << morda::Morda::inst().units.dpi() << std::endl)
//		TRACE(<< "id = " << this->dom->id << std::endl)
		svgren::Parameters svgParams;
		svgParams.dpi = morda::Morda::inst().units.dpi();
		svgParams.widthRequest = width;
		svgParams.heightRequest = height;
		auto svg = svgren::render(*this->dom, svgParams);
		ASSERT(svg.width != 0)
		ASSERT(svg.height != 0)
		ASSERT_INFO(svg.width * svg.height == svg.pixels.size(), "imWidth = " << svg.width << " imHeight = " << svg.height << " pixels.size() = " << svg.pixels.size())
		
		auto img = std::make_shared<SvgTexture>(
				this->sharedFromThis(this),
				morda::inst().renderer().factory->createTexture2D(kolme::Vec2ui(svg.width, svg.height), utki::wrapBuf(svg.pixels))
			);

		this->cache[std::make_tuple(svg.width, svg.height)] = img;

		return img;
	}
	
	mutable std::map<std::tuple<unsigned, unsigned>, std::weak_ptr<QuadTexture>> cache;
	
	static std::shared_ptr<ResSvgImage> load(const papki::File& fi){
		return std::make_shared<ResSvgImage>(svgdom::load(fi));
	}	
};
}



std::shared_ptr<ResImage> ResImage::load(const stob::Node& chain, const papki::File& fi) {
	if(auto f = chain.thisOrNext("file").get_node()){
		if(auto fn = f->child()){
			fi.setPath(fn->value());
			return ResImage::load(fi);
		}
	}
	
	return ResAtlasImage::load(chain, fi);
}

std::shared_ptr<ResImage> ResImage::load(const papki::File& fi) {
	if(fi.ext().compare("svg") == 0){
		return ResSvgImage::load(fi);
	}else{
		return ResRasterImage::load(fi);
	}
}
