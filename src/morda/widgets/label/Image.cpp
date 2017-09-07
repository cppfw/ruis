#include "Image.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"


using namespace morda;



Image::Image(const stob::Node* chain) :
		Widget(chain),
		BlendingWidget(chain)
{
	if(auto image = getProperty(chain, "image")){
		this->img = Morda::inst().resMan.load<ResImage>(image->value());
		this->resize(this->img->dim());
	}
	
	if(auto n = getProperty(chain, "keepAspectRatio")){
		this->keepAspectRatio = n->asBool();
	}else{
		this->keepAspectRatio = false;
	}
	
	if(auto n = getProperty(chain, "repeatX")){
		this->repeat_v.x = n->asBool();
	}else{
		this->repeat_v.x = false;
	}
	
	if(auto n = getProperty(chain, "repeatY")){
		this->repeat_v.y = n->asBool();
	}else{
		this->repeat_v.y = false;
	}
}

namespace{
const std::array<kolme::Vec2f, 4> quadFanTexCoords = {{
	kolme::Vec2f(0, 0), kolme::Vec2f(1, 0), kolme::Vec2f(1, 1), kolme::Vec2f(0, 1)
}};
}

void Image::render(const morda::Matr4r& matrix) const{
	if(!this->img){
		return;
	}

	this->applyBlending();
	
	auto& r = morda::inst().renderer();
	
	if(!this->scaledImage){
		this->scaledImage = this->img->get(this->rect().d);

		if(this->repeat_v.x || this->repeat_v.y){
			std::array<kolme::Vec2f, 4> texCoords;
			ASSERT(quadFanTexCoords.size() == texCoords.size())
			auto src = quadFanTexCoords.cbegin();
			auto dst = texCoords.begin();
			auto scale = this->rect().d.compDiv(this->img->dim());
			if(!this->repeat_v.x){
				scale.x = 1;
			}
			if(!this->repeat_v.y){
				scale.y = 1;
			}
			for(; dst != texCoords.end(); ++src, ++dst){
				*dst = src->compMul(scale);
			}
			this->vao = r.factory->createVertexArray({r.quad01VBO, r.factory->createVertexBuffer(utki::wrapBuf(texCoords))}, r.quadIndices, VertexArray::Mode_e::TRIANGLE_FAN);
		}else{
			this->vao = morda::inst().renderer().posTexQuad01VAO;
		}
	}
	ASSERT(this->scaledImage)

	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d);

	this->scaledImage->render(matr, *this->vao);
}

morda::Vec2r Image::measure(const morda::Vec2r& quotum)const{
	if(!this->img){
		return Vec2r(0);
	}
	
	Vec2r imgDim = this->img->dim(morda::Morda::inst().units.dpi());
	
	ASSERT_INFO(imgDim.isPositiveOrZero(), "imgDim = " << imgDim)
	
	if(!keepAspectRatio){
		Vec2r ret = imgDim;
		
		for(unsigned i = 0; i != ret.size(); ++i){
			if(quotum[i] >= 0){
				ret[i] = quotum[i];
			}
		}

		return ret;
	}
	
	ASSERT(this->img)
	ASSERT(imgDim.y > 0)
	
	real ratio = imgDim.x / imgDim.y;
	
	if(quotum.x < 0 && quotum.y < 0){
		return imgDim;
	}else if(quotum.x < 0){
		ASSERT(quotum.y >= 0)
		
		Vec2r ret;
		ret.y = quotum.y;
		ret.x = ratio * quotum.y;
		return ret;
	}else{
		ASSERT(quotum.x >= 0)
		ASSERT(quotum.y < 0)
		
		ASSERT(ratio > 0)
		
		Vec2r ret;
		ret.x = quotum.x;
		ret.y = quotum.x / ratio;
		return ret;
	}
}



void Image::setImage(const std::shared_ptr<const ResImage>& image) {
	if(this->img && image && this->img->dim() == image->dim()){
	}else{
		this->setRelayoutNeeded();
	}
	
	this->img = image;
	this->scaledImage.reset();
}

void Image::onResize() {
	this->Widget::onResize();
	this->scaledImage.reset();
}
