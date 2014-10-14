#include "../shaders/PosShader.hpp"

#include "Widget.hpp"

#include "../App.hpp"
#include "Container.hpp"
#include "../util/util.hpp"

using namespace morda;



Widget::Widget(const stob::Node* chain){
	if(const stob::Node* n = GetProperty(chain, "layout")){
		this->layout = n->CloneChain();
	}

	if(const stob::Node* n = GetProperty(chain, "posX")){
		this->rect.p.x = morda::DimValueFromSTOB(*n);
	}else{
		this->rect.p.x = 0;
	}
	
	if(const stob::Node* n = GetProperty(chain, "posY")){
		this->rect.p.y = morda::DimValueFromSTOB(*n);
	}else{
		this->rect.p.y = 0;
	}

	if(const stob::Node* n = GetProperty(chain, "dimX")){
		this->rect.d.x = morda::DimValueFromSTOB(*n);
	}else{
		this->rect.d.x = 0;
	}
	
	if(const stob::Node* n = GetProperty(chain, "dimY")){
		this->rect.d.y = morda::DimValueFromSTOB(*n);
	}else{
		this->rect.d.y = 0;
	}

	if(const stob::Node* p = GetProperty(chain, "name")){
		this->name = p->Value();
	}

	if(const stob::Node* p = GetProperty(chain, "clip")){
		this->clip = p->AsBool();
	}else{
		this->clip = false;
	}
	
	if(const stob::Node* p = GetProperty(chain, "visible")){
		this->isVisible = p->AsBool();
	}else{
		this->isVisible = true;
	}
	
	if(const stob::Node* p = GetProperty(chain, "enabled")){
		this->isEnabled = p->AsBool();
	}else{
		this->isEnabled = true;
	}
}


std::shared_ptr<Widget> Widget::FindChildByName(const std::string& name)NOEXCEPT{
	if(this->Name() == name){
		return this->SharedFromThis(this);
	}
	return nullptr;
}


std::unique_ptr<LayoutParams> Widget::ResetLayoutParams(std::unique_ptr<LayoutParams> params)NOEXCEPT{
	std::swap(this->layoutParams, params);
	this->SetRelayoutNeeded();
	return std::move(params);
}


void Widget::Resize(const morda::Vec2r& newDims){
	if(this->rect.d == newDims){
		return;
	}

	this->rect.d = newDims;
	ting::util::ClampBottom(this->rect.d.x, real(0.0f));
	ting::util::ClampBottom(this->rect.d.y, real(0.0f));
	this->OnResize();//call virtual method
	this->relayoutNeeded = false;
}



void Widget::RemoveFromParent(){
	if(this->parent){
		this->parent->Remove(*this);
	}
}



void Widget::SetRelayoutNeeded()NOEXCEPT{
	this->minDimNeedsRecomputing = true;
	if(this->relayoutNeeded){
		return;
	}
	this->relayoutNeeded = true;
	if(this->parent){
		this->parent->SetRelayoutNeeded();
	}
}



void Widget::RenderInternal(const morda::Matr4r& matrix)const{
	if(this->clip){
//		TRACE(<< "Widget::RenderInternal(): oldScissorBox = " << Rect2i(oldcissorBox[0], oldcissorBox[1], oldcissorBox[2], oldcissorBox[3]) << std::endl)

		//set scissor test
		Rect2i scissor(
				((matrix * Vec2r(0, 0) + Vec2r(1, 1)) / 2).CompMulBy(App::Inst().viewportDim()).Rounded().ConvertTo<int>(),
				this->rect.d.ConvertTo<int>()
			);

		Rect2i oldScissor;
		bool scissorTestWasEnabled = glIsEnabled(GL_SCISSOR_TEST) ? true : false;
		if(scissorTestWasEnabled){
			GLint osb[4];
			glGetIntegerv(GL_SCISSOR_BOX, osb);
			oldScissor = Rect2i(osb[0], osb[1], osb[2], osb[3]);
			scissor.Intersect(oldScissor);
		}else{
			glEnable(GL_SCISSOR_TEST);
		}
		
		glScissor(scissor.p.x, scissor.p.y, scissor.d.x, scissor.d.y);
		
		this->Render(matrix);
		
		if(scissorTestWasEnabled){
			glScissor(oldScissor.p.x, oldScissor.p.y, oldScissor.d.x, oldScissor.d.y);
		}else{
			glDisable(GL_SCISSOR_TEST);
		}
	}else{
		this->Render(matrix);
	}

	//render border
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	morda::ColorPosShader& s = App::Inst().Shaders().colorPosShader;
	s.Bind();
	morda::Matr4r matr(matrix);
	matr.Scale(this->Rect().d);
	s.SetMatrix(matr);

	if(this->IsHovered()){
		s.SetColor(morda::Vec3f(0, 1, 0));
	}else{
		s.SetColor(morda::Vec3f(1, 0, 1));
	}
	s.Render(s.quad01Fan, Shader::EMode::LINE_LOOP);
#endif
}



void Widget::OnKeyInternal(bool isDown, EKey keyCode){
	if(this->OnKey(isDown, keyCode)){
		return;
	}

	//pass key event to parent
	if(this->Parent()){
		this->Parent()->OnKeyInternal(isDown, keyCode);
	}
}



void Widget::Focus()NOEXCEPT{
	ASSERT(App::Inst().ThisIsUIThread())

	if(this->IsFocused()){
		return;
	}

	App::Inst().SetFocusedWidget(this->SharedFromThis(this));
}



void Widget::Unfocus()NOEXCEPT{
	ASSERT(App::Inst().ThisIsUIThread())

	if(!this->IsFocused()){
		return;
	}

	ASSERT(App::Inst().focusedWidget.lock() && App::Inst().focusedWidget.lock().operator->() == this)

	App::Inst().SetFocusedWidget(nullptr);
}



bool Widget::IsTopmost()const NOEXCEPT{
	if(!this->Parent()){
		return false;
	}
	
	ASSERT(this->Parent()->Children().size() != 0)
	
	return this->Parent()->Children().back().get() == this;
}



void Widget::MakeTopmost(){
	if(!this->Parent()){
		return;
	}
	
	ASSERT(this->Parent()->Children().size() != 0)
	
	this->Parent()->MakeChildTopmost(*this);
}
