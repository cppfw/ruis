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
