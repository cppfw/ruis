#include <memory>

#include <svgren/render.hpp>

//Some bad stuff defines OVERFLOW macro and there is an enum value with same name in svgdom/dom.h.
#ifdef OVERFLOW
#	undef OVERFLOW
#endif

#include <svgdom/dom.hpp>

#include "ResImage.hpp"

#include "../context.hpp"

#include "../util/util.hpp"

using namespace morda;

ResImage::ResImage(std::shared_ptr<morda::context> c) :
		resource(std::move(c))
{}


ResAtlasImage::ResAtlasImage(std::shared_ptr<morda::context> c, std::shared_ptr<ResTexture> tex, const Rectr& rect) :
		ResImage(std::move(c)),
		ResImage::QuadTexture(this->context->renderer, rect.d.abs()),
		tex(std::move(tex))
{
//	this->texCoords[3] = Vec2r(rect.left(), this->tex->tex().dim().y - rect.bottom()).compDivBy(this->tex->tex().dim());
//	this->texCoords[2] = Vec2r(rect.right(), this->tex->tex().dim().y - rect.bottom()).compDivBy(this->tex->tex().dim());
//	this->texCoords[1] = Vec2r(rect.right(), this->tex->tex().dim().y - rect.top()).compDivBy(this->tex->tex().dim());
//	this->texCoords[0] = Vec2r(rect.left(), this->tex->tex().dim().y - rect.top()).compDivBy(this->tex->tex().dim());
	//TODO:
	ASSERT(false)
}

ResAtlasImage::ResAtlasImage(std::shared_ptr<morda::context> c, std::shared_ptr<ResTexture> tex) :
		ResImage(std::move(c)),
		ResImage::QuadTexture(this->context->renderer, tex->tex().dims()),
		tex(std::move(tex)),
		vao(this->context->renderer->posTexQuad01VAO)
{}



std::shared_ptr<ResAtlasImage> ResAtlasImage::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi){
	std::shared_ptr<ResTexture> tex;
	Rectr rect(-1);

	for(auto& p : desc){
		if(p.value == "tex"){
			tex = ctx.loader.load<ResTexture>(get_property_value(p).to_string());
		}else if(p.value == "rect"){
			rect = parse_rect(p.children);
		}
	}

	if(!tex){
		throw std::runtime_error("ResAtlasImage::load(): could not load texture");
	}
	
	if(rect.p.x >= 0){
		return std::make_shared<ResAtlasImage>(ctx.shared_from_this(), tex, rect);
	}else{
		return std::make_shared<ResAtlasImage>(ctx.shared_from_this(), tex);
	}
}


void ResAtlasImage::render(const Matr4r& matrix, const VertexArray& vao) const {
	this->context->renderer->shader->posTex->render(matrix, *this->vao, this->tex->tex());
}



namespace{

class TexQuadTexture : public ResImage::QuadTexture{
protected:
	std::shared_ptr<Texture2D> tex_v;
	
	TexQuadTexture(std::shared_ptr<morda::renderer> r, std::shared_ptr<Texture2D> tex) :
			ResImage::QuadTexture(std::move(r), tex->dims()),
			tex_v(std::move(tex))
	{}
	
public:
	void render(const Matr4r& matrix, const VertexArray& vao) const override{
		this->renderer->shader->posTex->render(matrix, vao, *this->tex_v);
	}
};
	
class ResRasterImage : public ResImage, public TexQuadTexture{
public:
	ResRasterImage(std::shared_ptr<morda::context> c, std::shared_ptr<Texture2D> tex) :
			ResImage(std::move(c)),
			TexQuadTexture(this->context->renderer, std::move(tex))
	{}
	
	std::shared_ptr<const ResImage::QuadTexture> get(Vec2r forDim) const override{
		return this->sharedFromThis(this);
	}
	
	Vec2r dims(real dpi) const noexcept override{
		return this->tex_v->dims();
	}
	
	static std::shared_ptr<ResRasterImage> load(morda::context& ctx, const papki::file& fi){
		return std::make_shared<ResRasterImage>(ctx.shared_from_this(), loadTexture(*ctx.renderer, fi));
	}
};

class ResSvgImage : public ResImage{
	std::unique_ptr<svgdom::SvgElement> dom;
public:
	ResSvgImage(std::shared_ptr<morda::context> c, decltype(dom) dom) :
			ResImage(std::move(c)),
			dom(std::move(dom))
	{}
	
	Vec2r dims(real dpi)const noexcept override{
		auto wh = this->dom->getDimensions(dpi);
		return Vec2r(wh[0], wh[1]);
	}
	
	class SvgTexture : public TexQuadTexture{
		std::weak_ptr<const ResSvgImage> parent;
	public:
		SvgTexture(std::shared_ptr<morda::renderer> r, std::shared_ptr<const ResSvgImage> parent, std::shared_ptr<Texture2D> tex) :
				TexQuadTexture(std::move(r), std::move(tex)),
				parent(parent)
		{}

		~SvgTexture()noexcept{
			if(auto p = this->parent.lock()){
				r4::vec2ui d = this->tex_v->dims().to<unsigned>();
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

		{ // check if in cache
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
//		TRACE(<< "dpi = " << morda::gui::inst().units.dpi() << std::endl)
//		TRACE(<< "id = " << this->dom->id << std::endl)
		svgren::Parameters svgParams;
		svgParams.dpi = this->context->units.dots_per_inch;
		svgParams.widthRequest = width;
		svgParams.heightRequest = height;
		auto svg = svgren::render(*this->dom, svgParams);
		ASSERT(svg.width != 0)
		ASSERT(svg.height != 0)
		ASSERT_INFO(svg.width * svg.height == svg.pixels.size(), "imWidth = " << svg.width << " imHeight = " << svg.height << " pixels.size() = " << svg.pixels.size())
		
		auto img = std::make_shared<SvgTexture>(
				this->context->renderer,
				this->sharedFromThis(this),
				this->context->renderer->factory->createTexture2D(r4::vec2ui(svg.width, svg.height), utki::make_span(svg.pixels))
			);

		this->cache[std::make_tuple(svg.width, svg.height)] = img;

		return img;
	}
	
	mutable std::map<std::tuple<unsigned, unsigned>, std::weak_ptr<QuadTexture>> cache;
	
	static std::shared_ptr<ResSvgImage> load(morda::context& ctx, const papki::file& fi){
		return std::make_shared<ResSvgImage>(ctx.shared_from_this(), svgdom::load(fi));
	}	
};
}

std::shared_ptr<ResImage> ResImage::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi) {
	for(auto& p : desc){
		if(p.value == "file"){
			fi.setPath(get_property_value(p).to_string());
			return ResImage::load(ctx, fi);
		}
	}

	return ResAtlasImage::load(ctx, desc, fi);
}

std::shared_ptr<ResImage> ResImage::load(morda::context& ctx, const papki::file& fi) {
	if(fi.suffix().compare("svg") == 0){
		return ResSvgImage::load(ctx, fi);
	}else{
		return ResRasterImage::load(ctx, fi);
	}
}
