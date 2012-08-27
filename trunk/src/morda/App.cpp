#include "App.hpp"


#include <ting/debug.hpp>



using namespace morda;



ting::IntrusiveSingleton<App>::T_Instance App::instance;



void App::SetGLViewport(const tride::Vec2f& dim){
	glViewport(0, 0, dim.x, dim.y);
}



void App::Render(){
	if(this->rootContainer.IsNotValid()){
		return;
	}
	
	ASSERT(glGetError() == GL_NO_ERROR)

	glClearColor(0.0f, 0, 0.0f, 1.0f);
	ASSERT(glGetError() == GL_NO_ERROR)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ASSERT(glGetError() == GL_NO_ERROR)
	
	tride::Matr4f m;
	m.Identity();
	m.Translate(-1, -1);
	m.Scale(2.0f / this->curWinDim.x, 2.0f / this->curWinDim.y);
	
	this->rootContainer->Render(m);
	
	this->SwapGLBuffers();
}



void App::UpdateWindowDimensions(const tride::Vec2f& dim){
	if(this->curWinDim.x != dim.x || this->curWinDim.y != dim.y){
		this->curWinDim.x = dim.x;
		this->curWinDim.y = dim.y;

		this->SetGLViewport(this->curWinDim);
		if(this->rootContainer.IsValid()){
			this->rootContainer->Resize(this->curWinDim);
		}
	}
}
