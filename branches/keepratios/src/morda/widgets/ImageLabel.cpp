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
	return this->img->Dim();
}



void ImageLabel::SetImage(const std::shared_ptr<ResImage>& image) {
	if(this->img && image && this->img->Dim() != image->Dim()){
		this->SetRelayoutNeeded();
	}
	
	this->img = image;
}

