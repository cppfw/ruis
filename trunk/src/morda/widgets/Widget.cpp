#include "Widget.hpp"

#include "../App.hpp"
#include "Container.hpp"
#include "../shaders/SimpleSingleColoringShader.hpp"
#include "../util/util.hpp"

using namespace morda;



Widget::Widget(const stob::Node* desc){
	if(!desc){
		return;
	}
	
	if(const stob::Node* p = desc->Child("prop").node()){
		this->prop = p->Clone();
		this->prop->SetValue();//clear value of the prop node, we don't need it
	}

	if(const stob::Node* p = desc->GetProperty("pos")){
		this->rect.p = morda::DimVec2r(p);
	}

	if(const stob::Node* p = desc->GetProperty("dim")){
		this->rect.d = morda::DimVec2r(p);
	}

	if(const stob::Node* p = desc->GetProperty("name")){
		this->name = p->Value();
	}

	if(const stob::Node* p = desc->GetProperty("clip")){
		this->clip = p->AsBool();
	}
	
	if(const stob::Node* p = desc->GetProperty("background")){
		this->background = Background::New(*p);
	}
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
				RoundVec(
						((matrix * Vec2r(0, 0) + Vec2r(1, 1)) / 2)
								.CompMulBy(App::Inst().viewportDim())
					).ConvertTo<int>(),
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
		
		if(this->background){
			this->background->Render(matrix, this->Rect().d);
		}
		
		this->Render(matrix);
		
		if(scissorTestWasEnabled){
			glScissor(oldScissor.p.x, oldScissor.p.y, oldScissor.d.x, oldScissor.d.y);
		}else{
			glDisable(GL_SCISSOR_TEST);
		}
	}else{
		if(this->background){
			this->background->Render(matrix, this->Rect().d);
		}
		
		this->Render(matrix);
	}

	//render border
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	morda::SimpleSingleColoringShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	morda::Matr4r matr(matrix);
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
