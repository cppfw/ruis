#include "ImageLabel.hpp"

#include "../App.hpp"


using namespace morda;


ImageLabel::ImageLabel(const stob::Node& desc) :
		Widget(desc)
{
	if(const stob::Node* image = desc.GetProperty("image")){
		this->tex = App::Inst().ResMan().Load<ResTexture>(image->Value());
		this->Resize(this->tex->Tex().Dim());
	}
	
	if(const stob::Node* kar = desc.GetProperty("keepAspectRatio")){
		this->keepAspectRatio = kar->AsBool();
	}
}

void ImageLabel::Render(const morda::Matr4f& matrix) const{
	if(this->tex){
		morda::Matr4f matr(matrix);
		matr.Scale(this->Rect().d);

		this->tex->Tex().Bind();

		morda::SimpleTexturingShader &s = App::Inst().Shaders().simpleTexturing;
		s.Bind();
		s.EnablePositionPointer();
		s.SetMatrix(matr);
		s.DrawQuad01();
	}
}

morda::Vec2f ImageLabel::ComputeMinDim()const{
	if(!this->tex){
		return Vec2f(0);
	}
	return this->tex->Tex().Dim();
}


morda::Vec2f ImageLabel::Measure(const Vec2f& offer) const {
	if(!this->keepAspectRatio || !this->tex){
		return offer;
	}
	
	float texRatio = this->tex->Tex().Dim().x / this->tex->Tex().Dim().y;
	
	float offerRatio = offer.x / offer.y;
	
	if(texRatio <= offerRatio){
		return Vec2f(offer.y * texRatio, offer.y);
	}else{
		return Vec2f(offer.x, offer.x / texRatio);
	}
}

