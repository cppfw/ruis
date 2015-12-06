#include "ImageLabel.hpp"

#include "../../App.hpp"

#include "../../util/util.hpp"


using namespace morda;


ImageLabel::ImageLabel(const stob::Node* chain) :
		Widget(chain)
{
	if(auto image = getProperty(chain, "image")){
		this->img = App::inst().resMan.load<ResImage>(image->value());
		this->resize(this->img->dim());
	}
	
	if(auto n = getProperty(chain, "keepAspectRatio")){
		this->keepAspectRatio = n->asBool();
	}else{
		this->keepAspectRatio = false;
	}
}

void ImageLabel::render(const morda::Matr4r& matrix) const{
	if(!this->img){
		return;
	}
	
	morda::Matr4r matr(matrix);
	matr.scale(this->rect().d.compDiv(this->img->dim()));

	morda::PosTexShader &s = App::inst().shaders().posTexShader;
	
	this->img->render(matr, s);
}

morda::Vec2r ImageLabel::measure(const morda::Vec2r& quotum)const{
	if(!this->img){
		return Vec2r(0);
	}
	
	ASSERT(this->img->dim().isPositive())
	
	if(!keepAspectRatio){
		Vec2r ret = this->img->dim();
		
		for(unsigned i = 0; i != ret.size(); ++i){
			if(quotum[i] >= 0){
				ret[i] = quotum[i];
			}
		}

		return ret;
	}
	
	ASSERT(this->img)
	ASSERT(this->img->dim().y > 0)
	
	real ratio = this->img->dim().x / this->img->dim().y;
	
	if(quotum.x < 0 && quotum.y < 0){
		return this->img->dim();
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



void ImageLabel::SetImage(const std::shared_ptr<ResImage>& image) {
	if(this->img && image && this->img->dim() != image->dim()){
		this->setRelayoutNeeded();
	}
	
	this->img = image;
}

