#include "Slider.hpp"

#include "../App.hpp"
#include "../util/util.hpp"

#include <ting/math.hpp>
#include <ting/util.hpp>


using namespace morda;



void Slider::SliderHandle::Render(const morda::Matr4f& matrix)const{
	morda::Matr4f matr(matrix);
	matr.Scale(this->Rect().d);

	SimpleSingleColoringShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	s.SetColor(morda::Vec3f(0, 1, 1));
	s.SetMatrix(matr);
	s.DrawQuad01();
}
	

bool Slider::SliderHandle::OnMouseButton(bool isDown, const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
	if(button != LEFT){
		return false;
	}

	if(isDown){
		ASSERT(!this->isGrabbed)
		this->isGrabbed = true;

		unsigned longIndex = this->slider.IsVertical() ? 1 : 0;
		this->clickPoint = pos[longIndex];

		return true;
	}else{
		ASSERT(this->isGrabbed)
		this->isGrabbed = false;
		return true;
	}
}
	

bool Slider::SliderHandle::OnMouseMove(const morda::Vec2f& pos, unsigned pointerId){
	if(!this->isGrabbed){
		return false;
	}

	unsigned longIndex = morda::GetLongIndex(this->slider.IsVertical());

	float maxPos = this->slider.Rect().d[longIndex] - this->Rect().d[longIndex];
	ASSERT(maxPos >= 0)

	float newPos = this->Rect().p[longIndex];
	newPos += pos[longIndex] - this->clickPoint;
	ting::util::ClampRange(newPos, 0.0f, maxPos);

	morda::Vec2f newPosition(0);
	newPosition[longIndex] = newPos;
	
	this->MoveTo(newPosition);
	
	//update factor
	this->slider.curFactor = newPos / maxPos;
	this->slider.factorChange.Emit(this->slider.curFactor);
	
	return true;
}



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
	
	
	//TODO: "reverse"
}



void Slider::SetFactor(float newFactor){
	this->curFactor = ting::util::ClampedRange(newFactor, 0.0f, 1.0f);
	
	this->Slider::OnResize();
	
	this->factorChange.Emit(this->curFactor);
}



void Slider::OnResize(){
	unsigned longIndex = this->isVertical ? 1 : 0;
	
	morda::Vec2f newSize(this->Rect().d);
	
	newSize[longIndex] = ting::math::Round(newSize[longIndex] * this->handleSizeFactor);
	ting::util::ClampBottom(newSize[longIndex], this->GetMinDim()[longIndex]);
	
	this->Children()->Resize(newSize);
	
	//move
	{
		float effectiveLength = this->Rect().d[longIndex] - this->Children()->Rect().d[longIndex];
		morda::Vec2f newPos(0);
		newPos[longIndex] = ting::math::Round(effectiveLength * this->curFactor);
		this->Children()->MoveTo(newPos);
	}
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
	