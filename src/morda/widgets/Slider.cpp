#include "Slider.hpp"

#include "../App.hpp"

#include <ting/math.hpp>


using namespace morda;



namespace{

class SliderHandle : public morda::Widget{
	Slider& slider;
	
protected:
	SliderHandle(Slider& slider) :
			slider(slider)
	{
		this->MoveTo(0);
	}
public:
	static ting::Ref<SliderHandle> New(Slider& slider){
		return ting::Ref<SliderHandle>(new SliderHandle(slider));
	}
	
private:
	void Render(const morda::Matr4f& matrix) const OVERRIDE{
		morda::Matr4f matr(matrix);
		matr.Scale(this->Rect().d);

		SimpleSingleColoringShader& s = App::Inst().Shaders().simpleSingleColoring;
		s.Bind();
		s.SetColor(morda::Vec3f(0, 1, 1));
		s.SetMatrix(matr);
		s.DrawQuad01();
	}
};

}//~namespace



Slider::Slider(){
	this->Add(SliderHandle::New(*this));
}



Slider::Slider(const stob::Node& description) :
		Widget(description)
{
	this->Add(SliderHandle::New(*this));
	
	if(const stob::Node* n = description.GetProperty("vertical")){
		this->isVertical = n->AsBool();
	}
	
	
	//TODO:
}



void Slider::OnResize(){
	unsigned longIndex = this->isVertical ? 1 : 0;
	
	morda::Vec2f ns(this->Rect().d);
	
	ns[longIndex] = ting::math::Round(ns[longIndex] * this->handleSizeFactor);
	ting::util::ClampBottom(ns[longIndex], this->GetMinDim()[longIndex]);
	
	this->Children()->Resize(ns);
	
	//TODO: move
}



morda::Vec2f Slider::ComputeMinDim()const throw(){
	return morda::Vec2f(30, 30);//TODO: what should it be???
}



void Slider::Render(const morda::Matr4f& matrix) const{
	morda::Matr4f matr(matrix);
	matr.Scale(this->Rect().d);
	
	SimpleSingleColoringShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	s.SetColor(morda::Vec3f(0.5, 0.5, 0));
	s.SetMatrix(matr);
	s.DrawQuad01();
	
	this->Container::Render(matrix);
}
	