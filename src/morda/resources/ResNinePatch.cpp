#include "ResNinePatch.hpp"
#include "../App.hpp"
#include "../util/util.hpp"


using namespace morda;



namespace{

class ResSubImage : public ResImage, public ResImage::QuadTexture{
	friend class ResImage;
	
	std::shared_ptr<ResImage::QuadTexture> tex;
	
	std::array<Vec2r, 4> texCoords;
	
	Vec2r dim_v;
	
public:
	ResSubImage(decltype(tex) tex, const Rectr& rect);
	
	ResSubImage(const ResSubImage& orig) = delete;
	ResSubImage& operator=(const ResSubImage& orig) = delete;
	
	Vec2r dim(real dpi) const noexcept override{
		return this->dim_v;
	}
	
	virtual std::shared_ptr<const ResImage::QuadTexture> get(Vec2r forDim)const override{
		return this->sharedFromThis(this);
	}
	
	void render(const Matr4r& matrix, PosTexShader& s, const std::array<kolme::Vec2f, 4>& texCoords) const override{
		ASSERT(this->tex)
		this->tex->render(matrix, s, texCoords);
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
