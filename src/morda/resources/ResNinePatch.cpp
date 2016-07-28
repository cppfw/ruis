#include "ResNinePatch.hpp"
#include "../App.hpp"
#include "../util/util.hpp"

#include <iomanip>

using namespace morda;



namespace{

class ResSubImage : public ResImage, public ResImage::QuadTexture{
	friend class ResImage;
	
	std::shared_ptr<const ResImage::QuadTexture> tex;
	
	std::array<Vec2r, 4> texCoords;
	
public:
	//rect is a rectangle on the texture, Y axis up.
	ResSubImage(decltype(tex) tex, const Rectr& rect) :
			ResImage::QuadTexture(rect.d),
			tex(std::move(tex))
	{
		this->texCoords[0] = rect.p.compDiv(this->tex->dim());
		this->texCoords[1] = rect.rightBottom().compDiv(this->tex->dim());
		this->texCoords[2] = rect.rightTop().compDiv(this->tex->dim());
		this->texCoords[3] = rect.leftTop().compDiv(this->tex->dim());
//		TRACE(<< "this->texCoords = (" << this->texCoords[0] << ", " << this->texCoords[1] << ", " << this->texCoords[2] << ", " << this->texCoords[3] << ")" << std::endl)
	}
	
	ResSubImage(const ResSubImage& orig) = delete;
	ResSubImage& operator=(const ResSubImage& orig) = delete;
	
	Vec2r dim(real dpi) const noexcept override{
		return this->ResImage::QuadTexture::dim();
	}
	
	virtual std::shared_ptr<const ResImage::QuadTexture> get(Vec2r forDim)const override{
		return this->sharedFromThis(this);
	}
	
	void render(PosTexShader& s, const std::array<kolme::Vec2f, 4>& texCoords) const override{
		ASSERT(this->tex)
		this->tex->render(s, this->texCoords);
	}
};

}


std::shared_ptr<ResNinePatch> ResNinePatch::load(const stob::Node& chain, const papki::File& fi){
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
	real mul = 1;
	{
		auto req = borders.begin();
		for(auto orig = this->borders_v.begin(); orig != this->borders_v.end(); ++orig, ++req){
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
	
//	TRACE(<< "this->image->dim() = " << std::setprecision(10) << this->image->dim() << std::endl)
//	TRACE(<< "mul = " << std::setprecision(10) << mul << std::endl)
	
	auto quadTex = this->image->get((this->image->dim() * mul).round());
//	TRACE(<< "quadTex->dim() = " << quadTex->dim() << std::endl)
	
	Vec2r actMul = quadTex->dim().compDiv(this->image->dim());

	
//	TRACE(<< "actMul = " << std::setprecision(10) << actMul << std::endl)
	
	Sidesr scaledBorders(this->borders_v);
	scaledBorders.left() *= actMul.x;
	scaledBorders.right() *= actMul.x;
	scaledBorders.top() *= actMul.y;
	scaledBorders.bottom() *= actMul.y;
	
//	TRACE(<< "scaledBorders = " << std::setprecision(10) << scaledBorders << std::endl)
	
	auto ret = utki::makeShared<ImageMatrix>(
			std::array<std::array<std::shared_ptr<const ResImage>, 3>, 3>({{
				{{
					utki::makeShared<ResSubImage>(quadTex, Rectr(
							0,
							std::round(quadTex->dim().y - scaledBorders.top()),
							scaledBorders.left(),
							scaledBorders.top())
						), //left top
					utki::makeShared<ResSubImage>(quadTex, Rectr(
							scaledBorders.left(),
							std::round(quadTex->dim().y - scaledBorders.top()),
							std::round(quadTex->dim().x - scaledBorders.left() - scaledBorders.right()),
							scaledBorders.top())
						), //top
					utki::makeShared<ResSubImage>(quadTex, Rectr(
							std::round(quadTex->dim().x - scaledBorders.right()),
							std::round(quadTex->dim().y - scaledBorders.top()),
							scaledBorders.right(),
							scaledBorders.top())
						) //right top
				}},
				{{
					utki::makeShared<ResSubImage>(quadTex, Rectr(
							0,
							scaledBorders.bottom(),
							scaledBorders.left(),
							std::round(quadTex->dim().y - scaledBorders.top() - scaledBorders.bottom()))
						), //left
					utki::makeShared<ResSubImage>(quadTex, Rectr(
							scaledBorders.left(),
							scaledBorders.bottom(),
							std::round(quadTex->dim().x - scaledBorders.left() - scaledBorders.right()),
							std::round(quadTex->dim().y - scaledBorders.top() - scaledBorders.bottom()))
						), //middle
					utki::makeShared<ResSubImage>(quadTex, Rectr(
							std::round(quadTex->dim().x - scaledBorders.right()),
							scaledBorders.bottom(),
							scaledBorders.right(),
							std::round(quadTex->dim().y - scaledBorders.top() - scaledBorders.bottom()))
						) //right
				}},
				{{
					utki::makeShared<ResSubImage>(quadTex, Rectr(
							0,
							0,
							scaledBorders.left(),
							scaledBorders.bottom())
						), //left bottom
					utki::makeShared<ResSubImage>(quadTex, Rectr(
							scaledBorders.left(),
							0,
							std::round(quadTex->dim().x - scaledBorders.left() - scaledBorders.right()),
							scaledBorders.bottom())
						), //bottom
					utki::makeShared<ResSubImage>(quadTex, Rectr(
							std::round(quadTex->dim().x - scaledBorders.right()),
							0,
							scaledBorders.right(),
							scaledBorders.bottom())
						) //right bottom
				}}
			}}),
			this->sharedFromThis(this),
			mul
		);
	
	this->cache[mul] = ret;
	return ret;
}
