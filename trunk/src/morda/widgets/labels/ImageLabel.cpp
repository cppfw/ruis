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
	
	if(auto n = GetProperty(chain, "fitX")){
		this->fitDim.x = morda::DimValueFromSTOB(*n);
	}else{
		this->fitDim.x = -1;
	}
	
	if(auto n = GetProperty(chain, "fitY")){
		this->fitDim.y = morda::DimValueFromSTOB(*n);
	}else{
		this->fitDim.y = -1;
	}
}

void ImageLabel::Render(const morda::Matr4r& matrix) const{
	if(!this->img){
		return;
	}
	
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d.CompDiv(this->img->Dim()));

	morda::PosTexShader &s = App::Inst().Shaders().posTexShader;
	s.Bind();
	
	this->img->Render(matr, s);
}

morda::Vec2r ImageLabel::Measure(const morda::Vec2r& quotum)const{
	if(!this->img || this->fitDim.x == 0 || this->fitDim.y == 0){
		return Vec2r(0);
	}
	
	ASSERT(this->img->Dim().IsPositive())
	
	Vec2r ret = this->img->Dim();
	
	if(this->fitDim.x > 0){
		ret.x = this->fitDim.x;
	}
	
	if(this->fitDim.y > 0){
		ret.y = this->fitDim.y;
	}
	
	real ratio = this->img->Dim().x / this->img->Dim().y;
	
	real offeredRatio = ret.x / ret.y;
	
	if(ratio < offeredRatio){
		ret.x = ret.y * ratio;
	}else{
		ret.y = ret.x / ratio;
	}
	
	return ret;
}



void ImageLabel::SetImage(const std::shared_ptr<ResImage>& image) {
	if(this->img && image && this->img->Dim() != image->Dim()){
		this->SetRelayoutNeeded();
	}
	
	this->img = image;
}

