/* The MIT License:

Copyright (c) 2012-2014 Ivan Gagis <igagis@gmail.com>

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
#include <ting/config.hpp>
#include <ting/Buffer.hpp>
#include <ting/fs/File.hpp>
#include <ting/mt/Thread.hpp>

#include <memory>

#include "config.hpp"


#ifdef M_MORDA_OGLES2
#	include <GLES2/gl2.h>
#	include <EGL/egl.h>
#else
#	include <GL/glew.h>
#	if M_OS == M_OS_LINUX || M_OS == M_OS_MACOSX
#		include <GL/glx.h>
#	endif
#endif

#if M_OS == M_OS_LINUX
#	include <ting/mt/Queue.hpp>
#endif


#include "Exc.hpp"
#include "widgets/Widget.hpp"
#include "Inflater.hpp"
#include "Updateable.hpp"
#include "util/keycodes.hpp"
#include "util/CharInputFocusable.hpp"
#include "util/Vector2.hpp"
#include "ResourceManager.hpp"

#include "shaders/SimpleSingleColoringShader.hpp"
#include "shaders/SimpleTexturingShader.hpp"



namespace morda{



class App : public ting::IntrusiveSingleton<App>{
	friend class ting::IntrusiveSingleton<App>;
	static ting::IntrusiveSingleton<App>::T_Instance instance;

	friend class Updateable;
	friend class Widget;
	friend class CharInputFocusable;
	
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
	
	
	bool ThisIsUIThread()const NOEXCEPT{
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
		~EGLDisplayWrapper()NOEXCEPT;
	} eglDisplay;

	struct EGLConfigWrapper{
		EGLConfig c;
		EGLConfigWrapper(const WindowParams& wp, EGLDisplayWrapper& d);
		~EGLConfigWrapper()NOEXCEPT{}
	} eglConfig;

	struct EGLSurfaceWrapper{
		EGLDisplayWrapper& d;
		EGLSurface s;
		EGLSurfaceWrapper(EGLDisplayWrapper&d, EGLConfigWrapper& c);
		~EGLSurfaceWrapper()NOEXCEPT;
	} eglSurface;

	struct EGLContextWrapper{
		EGLDisplayWrapper& d;
		EGLContext c;
		EGLContextWrapper(EGLDisplayWrapper& d, EGLConfigWrapper& config, EGLSurfaceWrapper& s);
		~EGLContextWrapper()NOEXCEPT;
	} eglContext;

	void SwapGLBuffers(){
		eglSwapBuffers(this->eglDisplay.d, this->eglSurface.s);
	}
#endif
	
#if M_OS == M_OS_LINUX

private:
	ting::mt::Queue uiQueue;
	
private:
#	if M_OS_NAME == M_OS_NAME_ANDROID
	friend void UpdateWindowRect(App& app, const morda::Rect2f& rect);
	friend void Render(App& app);
	friend std::uint32_t Update(App& app);
	friend void HandleInputEvents();
	friend void HandleCharacterInputEvent(std::vector<std::uint32_t> chars);
	friend void HandleQueueMessages(App& app);
	friend int GetUIQueueHandle(App& app);
	
#	else
	struct XDisplayWrapper{
		Display* d;
		XDisplayWrapper();
		~XDisplayWrapper()NOEXCEPT;
	} xDisplay;

	struct XVisualInfoWrapper{
		XVisualInfo *vi;
		XVisualInfoWrapper(const WindowParams& wp, XDisplayWrapper& xDisplay);
		~XVisualInfoWrapper()NOEXCEPT;
	} xVisualInfo;

	struct XWindowWrapper{
		::Window w;

		XDisplayWrapper& d;

		XWindowWrapper(const App::WindowParams& wp, XDisplayWrapper& xDisplay, XVisualInfoWrapper& xVisualInfo);
		~XWindowWrapper()NOEXCEPT;
	} xWindow;

	struct GLXContextWrapper{
		GLXContext glxContext;

		XDisplayWrapper& d;
		XWindowWrapper& w;

		GLXContextWrapper(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow, XVisualInfoWrapper& xVisualInfo);
		~GLXContextWrapper()NOEXCEPT{
			this->Destroy();
		}

		void Destroy()NOEXCEPT;
	} glxContex;

	struct XInputMethodWrapper{
		XIM xim;
		XIC xic;
		
		XDisplayWrapper& d;
		XWindowWrapper& w;
		
		XInputMethodWrapper(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow);
		~XInputMethodWrapper()NOEXCEPT{
			this->Destroy();
		}
		
		void Destroy()NOEXCEPT;
	} xInputMethod;

	friend void Main(int argc, const char** argv);
	void Exec();

	void SwapGLBuffers(){
		glXSwapBuffers(this->xDisplay.d, this->xWindow.w);
	}
#	endif
	
#elif M_OS == M_OS_WINDOWS
	
private:
	struct WindowClassWrapper{
		std::string name;
		
		WindowClassWrapper();
		~WindowClassWrapper()NOEXCEPT;
	} windowClass;
	
	struct WindowWrapper{
		HWND hwnd;
		
		WindowWrapper(const WindowParams& wp, const WindowClassWrapper& wc);
		~WindowWrapper()NOEXCEPT;
	} window;
	
	struct DeviceContextWrapper{
		const WindowWrapper& w;
		HDC hdc;
		
		DeviceContextWrapper(const WindowParams& wp, const WindowWrapper& w);
		~DeviceContextWrapper()NOEXCEPT{
			this->Destroy();
		}
		
	private:
		void Destroy()NOEXCEPT;
	} deviceContext;
	
	struct GLContextWrapper{
		HGLRC hrc;
		
		GLContextWrapper(const DeviceContextWrapper& dc);
		~GLContextWrapper()NOEXCEPT{
			this->Destroy();
		}
		
	private:
		void Destroy()NOEXCEPT;
	} glContext;

	void SwapGLBuffers(){
		SwapBuffers(this->deviceContext.hdc);
	}

	

	friend void Main(int argc, const char** argv);
	void Exec();
	friend bool HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& lres);

#elif M_OS == M_OS_MACOSX	
private:
	struct ApplicationObject{
		void* id;
		ApplicationObject();
		~ApplicationObject()NOEXCEPT;
	} applicationObject;
	
	struct WindowObject{
		void* id;
		WindowObject(const morda::App::WindowParams& wp);
		~WindowObject()NOEXCEPT;
	} windowObject;
	
	struct OpenGLContext{
		void *id;
		OpenGLContext(void* window);
		~OpenGLContext()NOEXCEPT{
			this->Destroy();
		}
		
		void Destroy()NOEXCEPT;
	} openGLContext;
	
	void SwapGLBuffers();
	
	friend void Main(int argc, const char** argv);
	void Exec();
	
#else
#	error "unsupported OS"
#endif

public:
#if M_OS == M_OS_WINDOWS
	void PostToUIThread_ts(std::function<void()>&& f);
#else
	void PostToUIThread_ts(std::function<void()>&& f){	
		this->uiQueue.PushMessage(std::move(f));
	}
#endif

private:
	Updateable::Updater updater;

private:
	DefaultShaders shaders;

public:
	DefaultShaders& Shaders()NOEXCEPT{
		return this->shaders;
	}

	std::unique_ptr<ting::fs::File> CreateResourceFileInterface(const std::string& path = std::string())const;

private:
	//this is a viewport rectangle in coordinates that are as follows: x grows right, y grows up.
	morda::Rect2f curWinRect;

public:
	const morda::Vec2f& viewportDim()const NOEXCEPT{
		return this->curWinRect.d;
	}

public:
	ResourceManager resMan;

private:
	std::shared_ptr<morda::Widget> rootWidget; //NOTE: this should go after resMan as it may hold references to some resources

private:
	Inflater guiInflater;

	void UpdateGLViewport();

	void UpdateWindowRect(const morda::Rect2f& rect);

	void Render();

	//pos is in usual window coordinates, y goes down.
	morda::Vec2f NativeWindowToRootCoordinates(const morda::Vec2f& pos)const NOEXCEPT{
		return morda::Vec2f(pos.x, this->curWinRect.d.y - pos.y - 1.0f);
	}
	
	//pos is in usual window coordinates, y goes down.
	void HandleMouseMove(const morda::Vec2f& pos, unsigned id);

	//pos is in usual window coordinates, y goes down.
	void HandleMouseButton(bool isDown, const morda::Vec2f& pos, Widget::EMouseButton button, unsigned id);
	
	void HandleMouseHover(bool isHovered);

protected:
	App(const WindowParams& requestedWindowParams);

public:

	virtual ~App()NOEXCEPT{}

	void SetRootWidget(const std::shared_ptr<morda::Widget>& w){
		this->rootWidget = w;
		
		this->rootWidget->MoveTo(morda::Vec2f(0));
		this->rootWidget->Resize(this->curWinRect.d);
	}

	Inflater& inflater()NOEXCEPT{
		return this->guiInflater;
	}
	
	void ShowVirtualKeyboard()NOEXCEPT;
	
	void HideVirtualKeyboard()NOEXCEPT;
	
private:
	std::weak_ptr<Widget> focusedWidget;
	std::weak_ptr<CharInputFocusable> focusedCharInput;
	
	//The idea with UnicodeResolver parameter is that we don't want to calculate the unicode unless it is really needed, thus postpone it
	//as much as possible.
	template <class UnicodeResolver> void HandleCharacterInput(const UnicodeResolver& unicodeResolver){
		if(auto c = this->focusedCharInput.lock()){
//			TRACE(<< "HandleCharacterInput(): there is a focused widget" << std::endl)
			c->OnCharacterInput(unicodeResolver.Resolve());
		}
	}
	
	void HandleKeyEvent(bool isDown, key::Key keyCode){
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
	
	
private:
	float dotsPerCm;
	
public:
	
	float DotsPerCm()const NOEXCEPT{
		return this->dotsPerCm;
	}
};



}//~namespace
