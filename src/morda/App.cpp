#include "App.hpp"


#include <ting/debug.hpp>



using namespace morda;



ting::IntrusiveSingleton<App>::T_Instance App::instance;



void App::SetGLViewport(const tride::Rect2f& rect){
	glViewport(int(rect.p.x), int(rect.p.y), int(rect.d.x), int(rect.d.y));
}



void App::Render(){
	TRACE(<< "App::Render(): invoked" << std::endl)
	if(this->rootContainer.IsNotValid()){
		TRACE(<< "App::Render(): root container is not valid" << std::endl)
		return;
	}
	
	ASSERT(glGetError() == GL_NO_ERROR)

	glClearColor(0.0f, 0, 0.0f, 1.0f);
	ASSERT(glGetError() == GL_NO_ERROR)
			
	//TODO: clear depth if depth buffer is created, and stencil also, and whatever else
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ASSERT(glGetError() == GL_NO_ERROR)
	
	tride::Matr4f m;
	m.Identity();
	m.Translate(-1, -1);
	m.Scale(2.0f / this->curWinRect.d.x, 2.0f / this->curWinRect.d.y);
	
	this->rootContainer->Render(m);
	
	this->SwapGLBuffers();
}



void App::UpdateWindowRect(const tride::Rect2f& rect){
	if(this->curWinRect == rect){
		return;
	}
	
	this->curWinRect = rect;

	this->SetGLViewport(this->curWinRect);
	if(this->rootContainer.IsValid()){
		this->rootContainer->Resize(this->curWinRect.d);
	}
}
