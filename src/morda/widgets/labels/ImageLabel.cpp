#include "ImageLabel.hpp"

#include "../../App.hpp"

#include "../../util/util.hpp"


using namespace morda;


ImageLabel::ImageLabel(const stob::Node* chain) :
		Widget(chain)
{
	if(auto image = GetProperty(chain, "image")){
		this->img = App::Inst().resMan.Load<ResImage>(image->Value());
		this->Resize(this->img->Dim());
	}
	
	if(auto n = GetProperty(chain, "keepAspectRatio")){
		this->keepAspectRatio = n->AsBool();
	}else{
		this->keepAspectRatio = false;
	}
}

void ImageLabel::Render(const morda::Matr4r& matrix) const{
	if(!this->img){
		return;
	}
	
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d.CompDiv(this->img->Dim()));

	morda::PosTexShader &s = App::Inst().Shaders().posTexShader;
	
	this->img->Render(matr, s);
}

morda::Vec2r ImageLabel::onMeasure(const morda::Vec2r& quotum)const{
	if(!this->img){
		return Vec2r(0);
	}
	
	ASSERT(this->img->Dim().IsPositive())
	
	if(!keepAspectRatio){
		Vec2r ret = this->img->Dim();
		
		for(unsigned i = 0; i != ret.size(); ++i){
			if(quotum[i] >= 0){
				ret[i] = quotum[i];
			}
		}

		return ret;
	}
	
	ASSERT(this->img)
	ASSERT(this->img->Dim().y > 0)
	
	real ratio = this->img->Dim().x / this->img->Dim().y;
	
	if(quotum.x < 0 && quotum.y < 0){
		return this->img->Dim();
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
	if(this->img && image && this->img->Dim() != image->Dim()){
		this->SetRelayoutNeeded();
	}
	
	this->img = image;
}

