#include "Widget.hpp"

#include "App.hpp"
#include "Container.hpp"
#include "shaders/SimpleSingleColoringShader.hpp"
#include "util/util.hpp"

using namespace morda;



void Widget::ApplyDescription(const stob::Node& description, bool doNotCopyProp){
	if(!doNotCopyProp){
		if(const stob::Node* p = description.Child("prop").node()){
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

	if(const stob::Node* p = description.GetProperty("clip")){
		this->clip = p->AsBool();
	}
}



void Widget::RemoveFromParent(){
	if(ting::Ref<Container> p = this->parent){
		p->Remove(ting::Ref<Widget>(this));
	}
}



void Widget::SetRelayoutNeeded()throw(){
	this->minDimNeedsRecomputing = true;
	if(this->relayoutNeeded){
		return;
	}
	this->relayoutNeeded = true;
	if(ting::Ref<Container> p = this->parent){
		p->SetRelayoutNeeded();
	}
}



void Widget::RenderInternal(const morda::Matr4f& matrix)const{
	if(this->clip){
		GLint oldcissorBox[4];
		glGetIntegerv(GL_SCISSOR_BOX, oldcissorBox);

//		TRACE(<< "Widget::RenderInternal(): oldScissorBox = " << Rect2i(oldcissorBox[0], oldcissorBox[1], oldcissorBox[2], oldcissorBox[3]) << std::endl)

		//set scissor test
		const Vec2f& viewportDim = App::Inst().viewportDim();

		TRACE(<< " Widget::RenderInternal(): matrix = " << matrix << std::endl)
		Vec2f p0 = matrix * Vec2f(0, 0);
		p0 = (p0 + Vec2f(1, 1)) / 2;
		p0.CompMulBy(viewportDim);

		glScissor(p0.x, p0.y, this->rect.d.x, this->rect.d.y);
//		glScissor(0, 0, 0, 0);
		glEnable(GL_SCISSOR_TEST);

//		TRACE(<< " Widget::RenderInternal(): p0 = " << p0 << std::endl)
		//TODO:
	}else{
		//TODO:
	}

	this->Render(matrix);

	if(this->clip){
		glDisable(GL_SCISSOR_TEST);
	}

	//render border
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	morda::SimpleSingleColoringShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	morda::Matr4f matr(matrix);
	matr.Scale(this->Rect().d);
	s.SetMatrix(matr);

	if(this->IsHovered()){
		s.SetColor(morda::Vec3f(0, 1, 0));
	}else{
		s.SetColor(morda::Vec3f(1, 0, 1));
	}
	s.DrawQuad01(GL_LINE_LOOP);
#endif
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
