/* The MIT License:

Copyright (c) 2012 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <list>

#include <ting/Singleton.hpp>
#include <ting/types.hpp>
#include <ting/config.hpp>
#include <ting/Buffer.hpp>
#include <ting/Ptr.hpp>
#include <ting/fs/File.hpp>

#include "config.hpp"

#ifdef M_MORDA_OGLES2
#	include <GLES2/gl2.h>
#	include <EGL/egl.h>
#elif M_OS == M_OS_LINUX
#	include <GL/glew.h>
#	include <GL/glx.h>
#elif M_OS == M_OS_WINDOWS

#else
#	error "unknown os"
#endif


#include "util/Vector2.hpp"

#include "Exc.hpp"
#include "Widget.hpp"
#include "Container.hpp"
#include "GuiInflater.hpp"
#include "Updateable.hpp"
#include "KeyListener.hpp"
#include "resman/ResourceManager.hpp"

#include "shaders/SimpleSingleColoringShader.hpp"
#include "shaders/SimpleTexturingShader.hpp"
#include "widgets/AbstractButton.hpp"


namespace morda{



class App : public ting::IntrusiveSingleton<App>{
	friend class ting::IntrusiveSingleton<App>;
	static ting::IntrusiveSingleton<App>::T_Instance instance;

	friend class Updateable;
	
	friend class Widget;
	
	struct ThreadId{
		ting::mt::Thread::T_ThreadID id;
		ThreadId() :
				id(ting::mt::Thread::GetCurrentThreadID())
		{}
	} uiThreadId;

public:
	struct WindowParams{
		morda::Vec2ui dim;
	};
	
	
	inline bool ThisIsUIThread()const throw(){
		return this->uiThreadId.id == ting::mt::Thread::GetCurrentThreadID();
	}
	
	struct DefaultShaders{
		SimpleSingleColoringShader simpleSingleColoring;
		SimpleTexturingShader simpleTexturing;
	};



#ifdef M_MORDA_OGLES2

private:

	struct EGLDisplayWrapper{
		EGLDisplay d;
		EGLDisplayWrapper();
		~EGLDisplayWrapper()throw();
	} eglDisplay;

	struct EGLConfigWrapper{
		EGLConfig c;
		EGLConfigWrapper(const WindowParams& wp, EGLDisplayWrapper& d);
		~EGLConfigWrapper()throw(){}
	} eglConfig;

	struct EGLSurfaceWrapper{
		EGLDisplayWrapper& d;
		EGLSurface s;
		EGLSurfaceWrapper(EGLDisplayWrapper&d, EGLConfigWrapper& c);
		~EGLSurfaceWrapper()throw();
	} eglSurface;

	struct EGLContextWrapper{
		EGLDisplayWrapper& d;
		EGLContext c;
		EGLContextWrapper(EGLDisplayWrapper& d, EGLConfigWrapper& config, EGLSurfaceWrapper& s);
		~EGLContextWrapper()throw();
	} eglContext;

#	ifdef __ANDROID__

	friend void UpdateWindowRect(App& app, const morda::Rect2f& rect);
	friend void Render(App& app);
	friend ting::u32 Update(App& app);
	friend void HandleInputEvents();
	friend void HandleCharacterInputEvent(ting::Array<ting::u32> chars);

#	endif

	inline void SwapGLBuffers(){
		eglSwapBuffers(this->eglDisplay.d, this->eglSurface.s);
	}	
	
#elif M_OS == M_OS_LINUX

private:
	struct XDisplayWrapper{
		Display* d;
		XDisplayWrapper();
		~XDisplayWrapper()throw();
	} xDisplay;

	struct XVisualInfoWrapper{
		XVisualInfo *vi;
		XVisualInfoWrapper(const WindowParams& wp, XDisplayWrapper& xDisplay);
		~XVisualInfoWrapper()throw();
	} xVisualInfo;

	struct XWindowWrapper{
		Window w;

		XDisplayWrapper& d;

		XWindowWrapper(const App::WindowParams& wp, XDisplayWrapper& xDisplay, XVisualInfoWrapper& xVisualInfo);
		~XWindowWrapper()throw();
	} xWindow;

	struct GLXContextWrapper{
		GLXContext glxContext;

		XDisplayWrapper& d;
		XWindowWrapper& w;

		GLXContextWrapper(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow, XVisualInfoWrapper& xVisualInfo);
		~GLXContextWrapper()throw(){
			this->Destroy();
		}

		void Destroy()throw();
	} glxContex;

	struct XInputMethodWrapper{
		XIM xim;
		XIC xic;
		
		XDisplayWrapper& d;
		XWindowWrapper& w;
		
		XInputMethodWrapper(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow);
		~XInputMethodWrapper()throw(){
			this->Destroy();
		}
		
		void Destroy()throw();
	} xInputMethod;

	friend void Main(int argc, char** argv);
	void Exec();

	inline void SwapGLBuffers(){
		glXSwapBuffers(this->xDisplay.d, this->xWindow.w);
	}

#elif M_OS == M_OS_WINDOWS
	
private:
	struct WindowClassWrapper{
		std::string name;
		
		WindowClassWrapper();
		~WindowClassWrapper()throw();
	} windowClass;
	
	struct WindowWrapper{
		HWND hwnd;
		
		WindowWrapper(const WindowParams& wp, const WindowClassWrapper& wc);
		~WindowWrapper()throw();
	} window;
	
	struct DeviceContextWrapper{
		const WindowWrapper& w;
		HDC hdc;
		
		DeviceContextWrapper(const WindowParams& wp, const WindowWrapper& w);
		~DeviceContextWrapper()throw(){
			this->Destroy();
		}
		
	private:
		void Destroy()throw();
	} deviceContext;
	
	struct GLContextWrapper{
		HGLRC hrc;
		
		GLContextWrapper(const DeviceContextWrapper& dc);
		~GLContextWrapper()throw(){
			this->Destroy();
		}
		
	private:
		void Destroy()throw();
	} glContext;

	inline void SwapGLBuffers(){
		SwapBuffers(this->deviceContext.hdc);
	}

	friend void Main(int argc, char** argv);
	void Exec();
	friend bool HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& lres);
	
#else
#	error "unsupported OS"
#endif


private:
	Updateable::Updater updater;

private:
	DefaultShaders shaders;

public:
	inline DefaultShaders& Shaders()throw(){
		return this->shaders;
	}

	ting::Ptr<ting::fs::File> CreateResourceFileInterface(const std::string& path = std::string())const;

private:
	//this is a viewport rectangle in coordinates that are x grows right, y grows up.
	morda::Rect2f curWinRect;

	ting::Ref<morda::Container> rootContainer;

	ResourceManager resMan;

	GuiInflater inflater;

	void UpdateGLViewport();

	void UpdateWindowRect(const morda::Rect2f& rect);

	void Render();

	//pos is in usual window coordinates, y goes down.
	void HandleMouseMove(const morda::Vec2f& pos, unsigned id){
		if(this->rootContainer.IsNotValid()){
			return;
		}
		this->rootContainer->OnMouseMove(morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y - 1.0f), id);
	}

	//pos is in usual window coordinates, y goes down.
	void HandleMouseButtonDown(const morda::Vec2f& pos, Widget::EMouseButton button, unsigned id){
		if(this->rootContainer.IsNotValid()){
			return;
		}
		this->rootContainer->OnMouseButtonDown(morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y - 1.0f), button, id);
	}

		//pos is in usual window coordinates, y goes down.
	void HandleMouseButtonUp(const morda::Vec2f& pos, Widget::EMouseButton button, unsigned id){
		if(this->rootContainer.IsNotValid()){
			return;
		}
		this->rootContainer->OnMouseButtonUp(morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y - 1.0f), button, id);
	}
protected:
	App(const WindowParams& requestedWindowParams);

public:

	virtual ~App()throw(){}

	inline void SetRootContainer(const ting::Ref<morda::Container>& c){
		this->rootContainer = c;
		
		this->rootContainer->MoveTo(morda::Vec2f(0));
		this->rootContainer->Resize(this->curWinRect.d);
	}

	inline ResourceManager& ResMan()throw(){
		return this->resMan;
	}

	inline GuiInflater& Inflater()throw(){
		return this->inflater;
	}
	
	void ShowVirtualKeyboard()throw();
	
	void HideVirtualKeyboard()throw();
	
private:
	ting::WeakRef<Widget> focusedWidget;
	
	//The idea with UnicodeResolver parameter is that we don't want to calculate the unicode unless it is really needed, thus postpone it
	//as much as possible.
	template <bool is_down, bool is_char_input_only, class UnicodeResolver> void HandleKeyEvent(key::Key keyCode, UnicodeResolver& unicodeResolver){
//		TRACE(<< "HandleKeyEvent(): is_down = " << is_down << " is_char_input_only = " << is_char_input_only << " keyCode = " << unsigned(keyCode) << std::endl)
		
		//don't handle char input key release events
		if(!is_down && is_char_input_only){
			return;
		}
		
		if(ting::Ref<Widget> w = this->focusedWidget){
//			TRACE(<< "HandleKeyEvent(): there is a focused widget" << std::endl)
		
			if(!is_char_input_only){
				w->HandleKeyEvent<is_down>(keyCode);
			}
			
			if(is_down){
				w->HandleCharacterInput(unicodeResolver);
			}
		}else if(!is_char_input_only){
//			TRACE(<< "HandleKeyEvent(): there is no focused widget, not a char only input, passing to rootContainer" << std::endl)
			if(this->rootContainer){
				this->rootContainer->HandleKeyEvent<is_down>(keyCode);
			}
		}
	}
public:
	
};



/**
 * @brief Create application instance
 * User needs to define this factory function to create his application instance.
 * @return New application instance.
 */
ting::Ptr<App> CreateApp(int argc, char** argv, const ting::Buffer<const ting::u8>& savedState);



}//~namespace
