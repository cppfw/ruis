#include "App.hpp"


#include <ting/debug.hpp>
#include <ting/timer.hpp>



using namespace morda;



ting::IntrusiveSingleton<App>::T_Instance App::instance;



void App::UpdateGLViewport(){
	TRACE(<< "App::UpdateGLViewport(): this->curWinRect = " << this->curWinRect << std::endl)
	glViewport(
			int(this->curWinRect.p.x),
			int(this->curWinRect.p.y),
			int(this->curWinRect.d.x),
			int(this->curWinRect.d.y)
		);
}



void App::Render(){
	//TODO: render only if needed?
	
//	TRACE(<< "App::Render(): invoked" << std::endl)
	if(!this->rootWidget){
		TRACE(<< "App::Render(): root container is not valid" << std::endl)
		return;
	}
	
	ASSERT(glGetError() == GL_NO_ERROR)

	glClearColor(0.0f, 0, 0.0f, 1.0f);
	ASSERT(glGetError() == GL_NO_ERROR)
	
	//TODO: clear depth if depth buffer is created, and stencil also, and whatever else
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ASSERT(glGetError() == GL_NO_ERROR)
	
	morda::Matr4f m;
	m.Identity();
	m.Translate(-1, -1);
	m.Scale(2.0f / this->curWinRect.d.x, 2.0f / this->curWinRect.d.y);
	
	if(this->rootWidget->relayoutNeeded){
		this->rootWidget->OnResize();
	}
	
	this->rootWidget->RenderInternal(m);
	
	this->SwapGLBuffers();
}



void App::UpdateWindowRect(const morda::Rect2f& rect){
	if(this->curWinRect == rect){
		return;
	}
	
	this->curWinRect = rect;

	this->UpdateGLViewport();
	
	if(!this->rootWidget){
		return;
	}
	
	this->rootWidget->Resize(this->curWinRect.d);
}



void App::HandleMouseMove(const morda::Vec2f& pos, unsigned id){
	if(!this->rootWidget){
		return;
	}
	this->rootWidget->SetHovered(this->rootWidget->Rect().Overlaps(pos));
	this->rootWidget->OnMouseMove(this->NativeWindowToRootCoordinates(pos), id);
}



void App::HandleMouseButton(bool isDown, const morda::Vec2f& pos, Widget::EMouseButton button, unsigned id){
	if(!this->rootWidget){
		return;
	}

	this->rootWidget->SetHovered(this->rootWidget->Rect().Overlaps(pos));
	this->rootWidget->OnMouseButton(isDown, this->NativeWindowToRootCoordinates(pos), button, id);
}



void App::HandleMouseHover(bool isHovered){
	if(!this->rootWidget){
		return;
	}
	this->rootWidget->SetHovered(isHovered);
}



void App::PostToUIThread_ts(std::function<void()>&& f){
	class UIMessage : public ting::mt::Message{
		std::function<void()> f;
		virtual void Handle()override{
			ASSERT_ALWAYS(morda::App::Inst().ThisIsUIThread())
			this->f();
		}
		
	public:
		UIMessage(std::function<void()> f) :
				f(f)
		{}
	};
	
	this->uiQueue.PushMessage(ting::Ptr<ting::mt::Message>(new UIMessage(std::move(f))));
}
