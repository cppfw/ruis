#include "ImageLabel.hpp"

#include "../App.hpp"

#include "../util/util.hpp"


using namespace morda;


ImageLabel::ImageLabel(const stob::Node* chain) :
		Widget(chain)
{
	if(auto image = GetProperty(chain, "image")){
		this->img = App::Inst().resMan.Load<ResImage>(image->Value());
		this->Resize(this->img->Dim());
	}
	
	if(auto n = GetProperty(chain, "minDim")){
		ASSERT(n)
		if(auto x = n->ChildOfThisOrNext("x")){
			this->minDim.x = morda::DimValueFromSTOB(*x);
		}
		
		if(auto y = n->ChildOfThisOrNext("y")){
			this->minDim.y = morda::DimValueFromSTOB(*y);
		}
		
		if(auto k = n->ChildOfThisOrNext("keepRatio")){
			this->keepMinDimRatio = k->AsBool();
		}
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

morda::Vec2r ImageLabel::ComputeMinDim()const{
	if(!this->img){
		return Vec2r(0);
	}
	
	Vec2r ret = this->img->Dim();
	
	if(this->minDim.x >= 0){
		ret.x = this->minDim.x;
	}
	
	if(this->minDim.y >= 0){
		ret.y = this->minDim.y;
	}
	
	if(!this->keepMinDimRatio){
		return ret;
	}
	
	if(this->img->Dim().x == 0 || this->img->Dim().y == 0 || ret.x == 0 || ret.y == 0){
		return ret;
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

