#include "App.hpp"

#include "resources/ResSTOB.hpp"

#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>
#include <ting/fs/RootDirFile.hpp>



using namespace morda;



ting::IntrusiveSingleton<App>::T_Instance App::instance;



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



void App::UpdateWindowRect(const morda::Rect2f& rect){
	if(this->curWinRect == rect){
		return;
	}
	
	this->curWinRect = rect;

	TRACE(<< "App::UpdateGLViewport(): this->curWinRect = " << this->curWinRect << std::endl)
	Render::setViewport(Rect2i(
			int(this->curWinRect.p.x),
			int(this->curWinRect.p.y),
			int(this->curWinRect.d.x),
			int(this->curWinRect.d.y)
		));
	
	if(!this->rootWidget){
		return;
	}
	
	this->rootWidget->Resize(this->curWinRect.d);
}



void App::HandleMouseMove(const morda::Vec2r& pos, unsigned id){
	if(!this->rootWidget){
		return;
	}
	this->rootWidget->SetHovered(this->rootWidget->Rect().Overlaps(pos), id);
	this->rootWidget->OnMouseMove(this->NativeWindowToRootCoordinates(pos), id);
}



void App::HandleMouseButton(bool isDown, const morda::Vec2r& pos, Widget::EMouseButton button, unsigned pointerID){
	if(!this->rootWidget){
		return;
	}

	this->rootWidget->SetHovered(this->rootWidget->Rect().Overlaps(pos), pointerID);
	this->rootWidget->OnMouseButton(isDown, this->NativeWindowToRootCoordinates(pos), button, pointerID);
}



void App::HandleMouseHover(bool isHovered, unsigned pointerID){
	if(!this->rootWidget){
		return;
	}
	
	this->rootWidget->SetHovered(isHovered, pointerID);
}

void App::HandleKeyEvent(bool isDown, EKey keyCode){
	//		TRACE(<< "HandleKeyEvent(): is_down = " << is_down << " is_char_input_only = " << is_char_input_only << " keyCode = " << unsigned(keyCode) << std::endl)

	if(auto w = this->focusedWidget.lock()){
		//			TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		w->OnKeyInternal(isDown, keyCode);
	}else{
		//			TRACE(<< "HandleKeyEvent(): there is no focused widget, passing to rootWidget" << std::endl)
		if(this->rootWidget){
			this->rootWidget->OnKeyInternal(isDown, keyCode);
		}
	}
}




#if M_OS_NAME != M_OS_NAME_ANDROID
std::unique_ptr<ting::fs::File> App::CreateResourceFileInterface(const std::string& path)const{
	return ting::fs::FSFile::New(path);
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


#if (M_OS == M_OS_LINUX && M_OS_NAME != M_OS_NAME_ANDROID) || M_OS == M_OS_MACOSX

App::ResMan::ResMan(){
	//mount default resource pack
	
#	ifndef DEBUG
	unsigned soname = 
#	include "../soname.txt"
		;
#	endif
		
	std::vector<std::string> paths;
	
#	ifdef DEBUG
		paths.push_back("../../res/");
#	else
		{
			std::stringstream ss;
			ss << "/usr/local/share/morda/res" << soname << "/";
			paths.push_back(ss.str());
		}
		{
			std::stringstream ss;
			ss << "/usr/share/morda/res" << soname << "/";
			paths.push_back(ss.str());
		}
#	endif

	for(const auto& s : paths){
		try{
//			TRACE(<< "s = " << s << std::endl)
			this->MountResPack(*morda::App::Inst().CreateResourceFileInterface(s));
		}catch(ting::fs::File::Exc& e){
			continue;
		}
		break;
	}
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


App::Inflater::Inflater(){
	try{
		auto t = morda::App::Inst().resMan.Load<ResSTOB>("morda_gui_definitions");
		
		if(t->STOB()){
			this->Inflate(*t->STOB());
		}
		
	}catch(ResourceManager::Exc& e){
		//ignore
		TRACE(<< "App::Inflater::Inflater(): could not load morda_gui_definitions" << std::endl)
	}
}
