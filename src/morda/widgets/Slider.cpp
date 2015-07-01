#include "Slider.hpp"

#include "../App.hpp"
#include "../util/util.hpp"

#include <ting/math.hpp>
#include <ting/util.hpp>


using namespace morda;



void Slider::SliderHandle::Render(const morda::Matr4r& matrix)const{
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);

	ColorPosShader& s = App::Inst().Shaders().colorPosShader;
	s.Bind();
	s.SetColor(morda::Vec3f(0, 1, 1));
	s.SetMatrix(matr);
	s.render(s.quad01Fan);
}
	

bool Slider::SliderHandle::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
	if(button != EMouseButton::LEFT){
		return false;
	}

	if(isDown){
		ASSERT(!this->isGrabbed)
		this->isGrabbed = true;

		unsigned longIndex = this->slider.isVertical ? 1 : 0;
		this->clickPoint = pos[longIndex];

		return true;
	}else{
		if(this->isGrabbed){
			this->isGrabbed = false;
			return true;
		}else{
			return false;
		}
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
	
	if(this->slider.factorChange){
		this->slider.factorChange(this->slider);
	}
	
	return true;
}



Slider::Slider(bool isVertical, const stob::Node* chain) :
		Widget(chain),
		isVertical(isVertical)
{
	this->Add(ting::New<SliderHandle>(*this));
}



void Slider::SetFactor(float newFactor){
	real factor = ting::util::ClampedRange(newFactor, 0.0f, 1.0f);
	
	if(this->curFactor == factor){
		return;
	}
	
	this->curFactor = factor;
	
	this->Slider::OnResize();
	
	if(this->factorChange){
		this->factorChange(*this);
	}
}



void Slider::OnResize(){
	unsigned longIndex = this->GetLongIndex();
	
	morda::Vec2r newSize(this->Rect().d);
	
	newSize[longIndex] = ting::math::Round(newSize[longIndex] * this->handleSizeFactor);
	ting::util::ClampBottom(newSize[longIndex], this->measure(Vec2r(-1))[longIndex]);
	
	Widget& handle = *(*this->Children().begin());
	handle.Resize(newSize);
	
	//move
	{
		float effectiveLength = this->Rect().d[longIndex] - handle.Rect().d[longIndex];
		morda::Vec2r newPos(0);
		if(effectiveLength > 0){
			newPos[longIndex] = ting::math::Round(effectiveLength * this->curFactor);
			ASSERT(newPos[longIndex] <= effectiveLength)
		}
		handle.MoveTo(newPos);
	}
}



morda::Vec2r Slider::onMeasure(const morda::Vec2r& quotum)const NOEXCEPT{
	Vec2r ret = quotum;
	for(unsigned i = 0; i != ret.size(); ++i){
		if(ret[i] < 0){
			ret[i] = 30; //TODO: what should it be???
		}
	}
	return ret;
}



void Slider::Render(const morda::Matr4r& matrix) const{
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);
	
	ColorPosShader& s = App::Inst().Shaders().colorPosShader;
	s.Bind();
	s.SetColor(morda::Vec3f(0.5, 0.5, 0));
	s.SetMatrix(matr);
	s.render(s.quad01Fan);
	
	this->Container::Render(matrix);
}
	