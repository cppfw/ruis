#include "Widget.hpp"

#include "App.hpp"
#include "Container.hpp"
#include "shaders/SimpleSingleColoringShader.hpp"
#include "util/util.hpp"

using namespace morda;



void Widget::ApplyDescription(const stob::Node& description, bool doNotCopyProp){
	if(!doNotCopyProp){
		if(const stob::Node* p = description.Child("prop").second){
			this->prop = p->Clone();
			this->prop->SetValue();//clear value of the prop node, we don't need it
		}
	}

	if(const stob::Node* p = description.GetProperty("pos")){
		this->rect.p = morda::Vec2fFromSTOB(p);
	}else{
		this->rect.p.SetTo(0);
	}

	if(const stob::Node* p = description.GetProperty("dim")){
		this->rect.d = morda::Vec2fFromSTOB(p);
	}else{
		this->rect.d.SetTo(0);
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
	this->minDimNeedsRecomputing = true;
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



template <bool is_down> void Widget::PassKeyEventToParent(key::Key keyCode){
	if(ting::Ref<Container> p = this->Parent()){
		p->HandleKeyEvent<is_down>(keyCode);
	}else{
		App& app = App::Inst();
		if(app.rootContainer && static_cast<Widget*>(app.rootContainer.operator->()) != this){
			app.rootContainer->HandleKeyEvent<is_down>(keyCode);
		}
	}
}



void Widget::PassKeyUpEventToParent(key::Key keyCode){
	this->PassKeyEventToParent<false>(keyCode);
}



void Widget::PassKeyDownEventToParent(key::Key keyCode){
	this->PassKeyEventToParent<true>(keyCode);
}



void Widget::Focus()throw(){
	ASSERT(App::Inst().ThisIsUIThread())
	
	if(this->IsFocused()){
		return;
	}
	
	if(ting::Ref<Widget> w = App::Inst().focusedWidget){
		w->isFocused = false;
	}
	App::Inst().focusedWidget = ting::WeakRef<Widget>(this);
	this->isFocused = true;
}



void Widget::Unfocus()throw(){
	ASSERT(App::Inst().ThisIsUIThread())

	if(!this->IsFocused()){
		return;
	}
	
	ASSERT(App::Inst().focusedWidget.GetRef() && App::Inst().focusedWidget.GetRef().operator->() == this)
	
	App::Inst().focusedWidget.Reset();
	this->isFocused = false;
}
