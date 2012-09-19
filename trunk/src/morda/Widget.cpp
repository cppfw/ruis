#include "Widget.hpp"

#include "App.hpp"
#include "Container.hpp"
#include "shaders/SimpleSingleColoringShader.hpp"
#include "util/util.hpp"

using namespace morda;



void Widget::ApplyDescription(const stob::Node& description){
	if(const stob::Node* p = description.Child("prop").second){
		this->prop = p->Clone();
		this->prop->SetValue();//clear value of the prop node, we don't need it
	}

	if(const stob::Node* p = description.GetProperty("pos")){
		this->rect.p = morda::Vec2fFromSTOB(p);
	}else{
		this->rect.p.SetToZero();
	}

	if(const stob::Node* p = description.GetProperty("dim")){
		this->rect.d = morda::Vec2fFromSTOB(p);
	}

	if(const stob::Node* p = description.GetProperty("name")){
		this->name = p->Value();
	}
}



void Widget::RemoveFromParent(){
	if(ting::Ref<Container> p = this->parent){
		p->Remove(ting::Ref<Widget>(this));
	}
}



void Widget::RelayoutNeeded()throw(){
	if(this->relayoutNeeded){
		return;
	}
	this->relayoutNeeded = true;
	if(ting::Ref<Container> p = this->parent){
		p->RelayoutNeeded();
	}
}



void Widget::Render(const morda::Matr4f& matrix)const{
	//render border
	morda::SimpleSingleColoringShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	morda::Matr4f matr(matrix);
	matr.Scale(this->Rect().d);
	s.SetMatrix(matr);
	s.SetColor(morda::Vec3f(1, 0, 1));
	s.DrawQuad01(GL_LINE_LOOP);
}
