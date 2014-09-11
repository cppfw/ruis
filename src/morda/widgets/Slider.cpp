#include "Slider.hpp"

#include "../App.hpp"
#include "../util/util.hpp"

#include <ting/math.hpp>
#include <ting/util.hpp>


using namespace morda;



void Slider::SliderHandle::Render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);

	ColorPosShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	s.SetColor(morda::Vec3f(0, 1, 1));
	s.SetMatrix(matr);
	s.Render(s.quad01Fan, Shader::EMode::TRIANGLE_FAN);
}
	

bool Slider::SliderHandle::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
	if(button != EMouseButton::LEFT){
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
	

bool Slider::SliderHandle::OnMouseMove(const morda::Vec2r& pos, unsigned pointerId){
	if(!this->isGrabbed){
		return false;
	}

	unsigned longIndex = this->slider.GetLongIndex();

	float maxPos = this->slider.Rect().d[longIndex] - this->Rect().d[longIndex];
	ASSERT(maxPos >= 0)

	float newPos = this->Rect().p[longIndex];
	newPos += pos[longIndex] - this->clickPoint;
	ting::util::ClampRange(newPos, 0.0f, maxPos);

	morda::Vec2r newPosition(0);
	newPosition[longIndex] = newPos;
	
	this->MoveTo(newPosition);
	
	//update factor
	this->slider.curFactor = newPos / maxPos;
	if(this->slider.IsReverse()){
		this->slider.curFactor = 1 - this->slider.curFactor;
	}
	
	if(this->slider.factorChange){
		this->slider.factorChange(this->slider.curFactor);
	}
	
	return true;
}



Slider::Slider(const stob::Node* desc) :
		Widget(desc),
		LinearWidget(desc)
{
	this->Add(ting::New<SliderHandle>(*this));
}



void Slider::SetFactor(float newFactor){
	this->curFactor = ting::util::ClampedRange(newFactor, 0.0f, 1.0f);
	
	this->Slider::OnResize();
	
	if(this->factorChange){
		this->factorChange(this->curFactor);
	}
}



void Slider::OnResize(){
	unsigned longIndex = this->GetLongIndex();
	
	morda::Vec2r newSize(this->Rect().d);
	
	newSize[longIndex] = ting::math::Round(newSize[longIndex] * this->handleSizeFactor);
	ting::util::ClampBottom(newSize[longIndex], this->GetMinDim()[longIndex]);
	
	Widget& handle = *(*this->Children().begin());
	handle.Resize(newSize);
	
	//move
	{
		float effectiveLength = this->Rect().d[longIndex] - handle.Rect().d[longIndex];
		morda::Vec2r newPos(0);
		if(effectiveLength > 0){
			newPos[longIndex] = ting::math::Round(effectiveLength * this->curFactor);
			ASSERT(newPos[longIndex] <= effectiveLength)
			if(this->IsReverse()){
				newPos[longIndex] = effectiveLength - newPos[longIndex];
			}
		}
		handle.MoveTo(newPos);
	}
}



morda::Vec2r Slider::ComputeMinDim()const NOEXCEPT{
	return morda::Vec2r(30, 30);//TODO: what should it be???
}



void Slider::Render(const morda::Matr4r& matrix) const{
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);
	
	ColorPosShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	s.SetColor(morda::Vec3f(0.5, 0.5, 0));
	s.SetMatrix(matr);
	s.Render(s.quad01Fan, Shader::EMode::TRIANGLE_FAN);
	
	this->Container::Render(matrix);
}
	