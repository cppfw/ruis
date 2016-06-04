#include "ResNinePatch.hpp"
#include "../App.hpp"
#include "../util/util.hpp"


using namespace morda;



namespace{

class ResSubImage : public ResImage, public ResImage::QuadTexture{
	friend class ResImage;
	
	std::shared_ptr<const ResImage::QuadTexture> tex;
	
	std::array<Vec2r, 4> texCoords;
	
public:
	//rect is a rectangle on the texture, Y axis down.
	ResSubImage(decltype(tex) tex, const Rectr& rect) :
			ResImage::QuadTexture(rect.d),
			tex(std::move(tex))
	{
		this->texCoords[0] = rect.leftTop().compDiv(this->tex->dim());
		this->texCoords[1] = rect.rightTop().compDiv(this->tex->dim());
		this->texCoords[2] = rect.rightBottom().compDiv(this->tex->dim());
		this->texCoords[3] = rect.p.compDiv(this->tex->dim());
	}
	
	ResSubImage(const ResSubImage& orig) = delete;
	ResSubImage& operator=(const ResSubImage& orig) = delete;
	
	Vec2r dim(real dpi) const noexcept override{
		return this->ResImage::QuadTexture::dim();
	}
	
	virtual std::shared_ptr<const ResImage::QuadTexture> get(Vec2r forDim)const override{
		return this->sharedFromThis(this);
	}
	
	void render(const Matr4r& matrix, PosTexShader& s, const std::array<kolme::Vec2f, 4>& texCoords) const override{
		ASSERT(this->tex)
		this->tex->render(matrix, s, this->texCoords);
	}
};

}


std::shared_ptr<ResNinePatch> ResNinePatch::load(const stob::Node& chain, const papki::File& fi){
	
	//TODO: remove?
	if(chain.thisOrNext("tex").node()){
		auto tex = morda::App::inst().resMan.load<ResTexture>(chain.side("tex").up().value());

		auto rect = makeRectrFromSTOB(&chain.side("rect").up());

		auto borders = makeRectrFromSTOB(&chain.side("borders").up());

		return utki::makeShared<ResNinePatch>(
				Sidesr(borders.p.x, borders.p.y, borders.d.x, borders.d.y),
				utki::makeShared<ResAtlasImage>(tex, Rectr(rect.p, borders.p)), //lt
				utki::makeShared<ResAtlasImage>(tex, Rectr(rect.p.x + borders.p.x, rect.p.y, rect.d.x - borders.p.x - borders.d.x, borders.p.y)), //t
				utki::makeShared<ResAtlasImage>(tex, Rectr(rect.right() - borders.d.x, rect.p.y, borders.d.x, borders.p.y)), //rt
				utki::makeShared<ResAtlasImage>(tex, Rectr(rect.p.x, rect.p.y + borders.p.y, borders.p.x, rect.d.y - borders.p.y - borders.d.y)), //l
				utki::makeShared<ResAtlasImage>(tex, Rectr(rect.p.x + borders.p.x, rect.p.y + borders.p.y, rect.d.x - borders.p.x - borders.d.x, rect.d.y - borders.p.y - borders.d.y)), //m
				utki::makeShared<ResAtlasImage>(tex, Rectr(rect.right() - borders.d.x, rect.p.y + borders.p.y, borders.d.x, rect.d.y - borders.p.y - borders.d.y)), //r
				utki::makeShared<ResAtlasImage>(tex, Rectr(rect.p.x, rect.top() - borders.d.y, borders.p.x, borders.d.y)), //lb
				utki::makeShared<ResAtlasImage>(tex, Rectr(rect.p.x + borders.p.x, rect.top() - borders.d.y, rect.d.x - borders.p.x - borders.d.x, borders.d.y)), //b
				utki::makeShared<ResAtlasImage>(tex, Rectr(rect.right() - borders.d.x, rect.top() - borders.d.y, borders.d.x, borders.d.y)) //br
			);
	}
	
	auto borders = makeSidesrFromSTOB(&chain.side("borders").up());
	
	auto file = chain.side("file").up().asString();
	fi.setPath(file);
	auto image = ResImage::load(fi);
	
	return utki::makeShared<ResNinePatch>(image, borders);
}

ResNinePatch::ImageMatrix::ImageMatrix(std::array<std::array<std::shared_ptr<const ResImage>, 3>, 3>&& l, std::shared_ptr<const ResNinePatch> parent, real mul) :
		images_v(l),
		parent(parent),
		mul(mul)
{}

ResNinePatch::ImageMatrix::~ImageMatrix()noexcept{
	if(auto p = this->parent.lock()){		
		p->cache.erase(this->mul);
	}
}


std::shared_ptr<ResNinePatch::ImageMatrix> ResNinePatch::get(Sidesr borders) const {
	//TODO: remove
	if(!this->image){
		return utki::makeShared<ImageMatrix>(
				std::array<std::array<std::shared_ptr<const ResImage>, 3>, 3>({{
					{{this->lt, this->t, this->rt}},
					{{this->l, this->m, this->r}},
					{{this->lb, this->b, this->rb}}
				}}),
				this->sharedFromThis(this),
				real(1)
			);
	}
	
	real mul = 1;
	{
		auto req = borders.begin();
		for(auto orig = this->borders.begin(); orig != this->borders.end(); ++orig, ++req){
			if(*orig <= 0 || *req <= 0){
				continue;
			}

			if(*req > (*orig) * mul){
				mul = *req / *orig;
			}
		}
	}
	
	{
		auto i = this->cache.find(mul);
		if(i != this->cache.end()){
			if(auto r = i->second.lock()){
				return r;
			}
		}
	}
	
	auto quadTex = this->image->get(this->image->dim() * mul);
	
	Vec2r actMul = quadTex->dim().compDiv(this->image->dim());
	
	Sidesr scaledBorders(this->borders);
	scaledBorders.left() *= actMul.x;
	scaledBorders.right() *= actMul.x;
	scaledBorders.top() *= actMul.y;
	scaledBorders.bottom() *= actMul.y;
	
	auto ret = utki::makeShared<ImageMatrix>(
			std::array<std::array<std::shared_ptr<const ResImage>, 3>, 3>({{
				{{
					utki::makeShared<ResSubImage>(quadTex, Rectr(0, 0, scaledBorders.left(), scaledBorders.top())), //left top
					utki::makeShared<ResSubImage>(quadTex, Rectr(scaledBorders.left(), 0, quadTex->dim().x - scaledBorders.left() - scaledBorders.right(), scaledBorders.top())), //top
					utki::makeShared<ResSubImage>(quadTex, Rectr(quadTex->dim().x - scaledBorders.right(), 0, scaledBorders.right(), scaledBorders.top())) //right top
				}},
				{{
					utki::makeShared<ResSubImage>(quadTex, Rectr(0, scaledBorders.top(), scaledBorders.left(), quadTex->dim().y - scaledBorders.top() - scaledBorders.bottom())), //left
					utki::makeShared<ResSubImage>(quadTex, Rectr(scaledBorders.left(), scaledBorders.top(), quadTex->dim().x - scaledBorders.left() - scaledBorders.right(), quadTex->dim().y - scaledBorders.top() - scaledBorders.bottom())), //middle
					utki::makeShared<ResSubImage>(quadTex, Rectr(quadTex->dim().x - scaledBorders.right(), scaledBorders.top(), scaledBorders.right(), quadTex->dim().y - scaledBorders.top() - scaledBorders.bottom())) //right
				}},
				{{
					utki::makeShared<ResSubImage>(quadTex, Rectr(0, quadTex->dim().y - scaledBorders.bottom(), scaledBorders.left(), scaledBorders.bottom())), //left bottom
					utki::makeShared<ResSubImage>(quadTex, Rectr(scaledBorders.left(), quadTex->dim().y - scaledBorders.bottom(), quadTex->dim().x - scaledBorders.left() - scaledBorders.right(), scaledBorders.bottom())), //bottom
					utki::makeShared<ResSubImage>(quadTex, Rectr(quadTex->dim().x - scaledBorders.right(), quadTex->dim().y - scaledBorders.bottom(), scaledBorders.right(), scaledBorders.bottom())) //right bottom
				}}
			}}),
			this->sharedFromThis(this),
			mul
		);
	
	this->cache[mul] = ret;
	return ret;
}
