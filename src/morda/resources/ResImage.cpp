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

#include "../shaders/PosTexShader.hpp"


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
		return utki::makeShared<ResAtlasImage>(tex, rect);
	}else{
		return utki::makeShared<ResAtlasImage>(tex);
	}
}


void ResAtlasImage::render(const Matr4r& matrix, VertexArray& vao) const {
	morda::inst().renderer().shaderPosTex->render(matrix, this->tex->tex(), *this->vao);
}



namespace{

class TexQuadTexture : public ResImage::QuadTexture{
protected:
	std::shared_ptr<Texture2D_n> tex_v;
	
	TexQuadTexture(std::shared_ptr<Texture2D_n> tex) :
			ResImage::QuadTexture(tex->dim()),
			tex_v(std::move(tex))
	{}
	
public:
	void render(const Matr4r& matrix, VertexArray& vao) const override{
		morda::inst().renderer().shaderPosTex->render(matrix, *this->tex_v, vao);
	}
};
	
class ResRasterImage : public ResImage, public TexQuadTexture{
public:
	ResRasterImage(std::shared_ptr<Texture2D_n> tex) :
			TexQuadTexture(std::move(tex))
	{}
	
	std::shared_ptr<const ResImage::QuadTexture> get(Vec2r forDim) const override{
		return this->sharedFromThis(this);
	}
	
	Vec2r dim(real dpi) const noexcept override{
		return this->tex_v->dim();
	}
	
	static std::shared_ptr<ResRasterImage> load(const papki::File& fi){
		return utki::makeShared<ResRasterImage>(loadTexture_n(fi));
	}
};

class ResSvgImage : public ResImage{
	std::unique_ptr<svgdom::SvgElement> dom;
public:
	ResSvgImage(decltype(dom) dom) :
			dom(std::move(dom))
	{}
	
	Vec2r dim(real dpi)const noexcept override{
		return Vec2r(
				this->dom->width.toPx(dpi),
				this->dom->height.toPx(dpi)
			);
	}
	
	class SvgTexture : public TexQuadTexture{
		std::weak_ptr<const ResSvgImage> parent;
	public:
		SvgTexture(std::shared_ptr<const ResSvgImage> parent, std::shared_ptr<Texture2D_n> tex) :
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

		auto pixels = svgren::render(*this->dom, imWidth, imHeight, morda::Morda::inst().units.dpi());
		ASSERT_INFO(imWidth * imHeight == pixels.size(), "imWidth = " << imWidth << " imHeight = " << imHeight << " pixels.size() = " << pixels.size())
		
		//flip pixels vertically
		{
			std::vector<std::uint32_t> line(imWidth);

			size_t stride = imWidth * sizeof(pixels[0]);
			
			//TODO: use iterators
			for(unsigned i = 0; i != imHeight / 2; ++i){
				memcpy(&*line.begin(), &*pixels.begin() + imWidth * i, stride);//move line to temp
				memcpy(&*pixels.begin() + imWidth * i, &*pixels.begin() + imWidth * (imHeight - i - 1), stride);//move bottom line to top
				memcpy(&*pixels.begin() + imWidth * (imHeight - i - 1), &*line.begin(), stride);
			}
		}
		
		auto img = utki::makeShared<SvgTexture>(
				this->sharedFromThis(this),
				morda::inst().renderer().factory->createTexture2D(kolme::Vec2ui(imWidth, imHeight), utki::wrapBuf(pixels))
			);

		this->cache[std::make_tuple(imWidth, imHeight)] = img;

		return img;
	}
	
	mutable std::map<std::tuple<unsigned, unsigned>, std::weak_ptr<QuadTexture>> cache;
	
	static std::shared_ptr<ResSvgImage> load(const papki::File& fi){
		return utki::makeShared<ResSvgImage>(svgdom::load(fi));
	}	
};
}



std::shared_ptr<ResImage> ResImage::load(const stob::Node& chain, const papki::File& fi) {
	if(auto f = chain.thisOrNext("file").node()){
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
