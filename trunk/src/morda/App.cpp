#include "App.hpp"


#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>
#include <ting/fs/RootDirFile.hpp>



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
	
	morda::Matr4r m;
	m.Identity();
	m.Translate(-1, -1);
	m.Scale(2.0f / this->curWinRect.d.x, 2.0f / this->curWinRect.d.y);
	
	if(this->rootWidget->relayoutNeeded){
		this->rootWidget->OnResize();
	}
	
	this->rootWidget->RenderInternal(m);
	
	this->SwapGLBuffers();
}



void App::UpdateWindowRect(const morda::Rect2r& rect){
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



void App::HandleMouseMove(const morda::Vec2r& pos, unsigned id){
	if(!this->rootWidget){
		return;
	}
	this->rootWidget->SetHovered(this->rootWidget->Rect().Overlaps(pos));
	this->rootWidget->OnMouseMove(this->NativeWindowToRootCoordinates(pos), id);
}



void App::HandleMouseButton(bool isDown, const morda::Vec2r& pos, Widget::EMouseButton button, unsigned id){
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



#if M_OS_NAME != M_OS_NAME_ANDROID
std::unique_ptr<ting::fs::File> App::CreateResourceFileInterface(const std::string& path)const{
	return ting::fs::RootDirFile::New(ting::fs::FSFile::New(path), "res/");
}

void App::ShowVirtualKeyboard()NOEXCEPT{
	TRACE(<< "App::ShowVirtualKeyboard(): invoked" << std::endl)
	//do nothing
}



void App::HideVirtualKeyboard()NOEXCEPT{
	TRACE(<< "App::HideVirtualKeyboard(): invoked" << std::endl)
	//do nothing
}
#endif



void App::SetFocusedWidget(const std::shared_ptr<Widget> w){
	if(auto prev = this->focusedWidget.lock()){
		prev->isFocused = false;
		prev->OnFocusedChanged();
	}
	
	this->focusedWidget = w;
	
	if(w){
		w->isFocused = true;
		w->OnFocusedChanged();
	}
}
