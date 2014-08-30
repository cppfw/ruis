#include "ImageLabel.hpp"

#include "../App.hpp"


using namespace morda;


ImageLabel::ImageLabel(const stob::Node* desc) :
		Widget(desc)
{
	if(!desc){
		return;
	}
	
	if(const stob::Node* image = desc->GetProperty("image")){
		this->img = App::Inst().resMan.Load<ResImage>(image->Value());
		this->Resize(this->img->Dim());
	}
	
	if(const stob::Node* kar = desc->GetProperty("keepAspectRatio")){
		this->keepAspectRatio = kar->AsBool();
	}
}

void ImageLabel::Render(const morda::Matr4r& matrix) const{
	if(!this->img){
		return;
	}
	
	morda::Matr4r matr(matrix);
//	matr.Scale(this->Rect().d);

	morda::SimpleTexturingShader &s = App::Inst().Shaders().simpleTexturing;
	s.Bind();
	
	this->img->Render(matr, s);
}

morda::Vec2r ImageLabel::ComputeMinDim()const{
	if(!this->img){
		return Vec2r(0);
	}
	return this->img->Dim();
}


morda::Vec2r ImageLabel::Measure(const Vec2r& offer) const {
	if(!this->keepAspectRatio || !this->img){
		return offer;
	}
	
	float texRatio = this->img->Dim().x / this->img->Dim().y;
	
	float offerRatio = offer.x / offer.y;
	
	if(texRatio <= offerRatio){
		return Vec2r(offer.y * texRatio, offer.y);
	}else{
		return Vec2r(offer.x, offer.x / texRatio);
	}
}

