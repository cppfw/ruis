/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <utki/Singleton.hpp>
#include <utki/config.hpp>
#include <utki/Buf.hpp>
#include <utki/Unique.hpp>

#include <papki/File.hpp>

#include <nitki/Thread.hpp>

#include <kolme/Vector2.hpp>

#include <memory>

#include "config.hpp"


#if M_MORDA_RENDER == M_MORDA_RENDER_OPENGL
#	include <GL/glew.h>

#	if M_OS == M_OS_LINUX
#		include <GL/glx.h>
#	endif

#elif M_MORDA_RENDER == M_MORDA_RENDER_OPENGLES
#	if M_OS_NAME == M_OS_NAME_IOS
#		include <OpenGlES/ES2/glext.h>
#	else
#		include <GLES2/gl2.h>
#		include <EGL/egl.h>
#	endif
#elif M_MORDA_RENDER == M_MORDA_RENDER_DIRECTX

#else
#	error "Unknown OS"
#endif


#if M_OS == M_OS_LINUX || M_OS == M_OS_MACOSX
#	include <nitki/Queue.hpp>
#endif


#include "Exc.hpp"
#include "Inflater.hpp"
#include "Updateable.hpp"
#include "ResourceManager.hpp"

#include "util/keycodes.hpp"

#include "widgets/Widget.hpp"
#include "widgets/CharInputWidget.hpp"

#include "shaders/ColorPosShader.hpp"
#include "shaders/ColorPosTexShader.hpp"
#include "shaders/PosTexShader.hpp"
#include "shaders/SimpleGrayscalePosTexShader.hpp"
#include "shaders/SimpleBlurPosTexShader.hpp"



namespace morda{



class App : public utki::IntrusiveSingleton<App>, public utki::Unique{
	friend class utki::IntrusiveSingleton<App>;
	static utki::IntrusiveSingleton<App>::T_Instance instance;

	friend class Render;
	friend class Updateable;
	friend class Widget;
	friend class CharInputWidget;

	nitki::Thread::T_ThreadID uiThreadId = nitki::Thread::getCurrentThreadID();

public:
	struct WindowParams{
		kolme::Vec2ui dim;
	};


	bool thisIsUIThread()const noexcept{
		return this->uiThreadId == nitki::Thread::getCurrentThreadID();
	}

	struct DefaultShaders{
		ColorPosShader colorPosShader;
		ColorPosTexShader colorPosTexShader;
		PosTexShader posTexShader;
		SimpleGrayscalePosTexShader simpleGrayscalePosTexShader;
		SimpleBlurPosTexShader simpleBlurPosTexShader;
	};


#if M_OS == M_OS_LINUX

private:
#	if M_OS_NAME == M_OS_NAME_ANDROID
	friend void updateWindowRect(App& app, const morda::Rectr& rect);
	friend void Render(App& app);
	friend std::uint32_t Update(App& app);
	friend void HandleInputEvents();
	friend void HandleCharacterInputEvent(std::vector<std::uint32_t>&& chars);
	friend void HandleQueueMessages(App& app);
	friend int GetUIQueueHandle(App& app);

private:
	struct DotsPerInchWrapper{
		float value;
		
		DotsPerInchWrapper();
	} dotsPerInch_var;
	
	struct EGLDisplayWrapper{
		EGLDisplay d;
		EGLDisplayWrapper();
		~EGLDisplayWrapper()noexcept;
	} eglDisplay;
	
	struct EGLConfigWrapper{
		EGLConfig c;
		EGLConfigWrapper(const WindowParams& wp, EGLDisplayWrapper& d);
		~EGLConfigWrapper()noexcept{}
	} eglConfig;
	
	struct EGLSurfaceWrapper{
		EGLDisplayWrapper& d;
		EGLSurface s;
		EGLSurfaceWrapper(EGLDisplayWrapper&d, EGLConfigWrapper& c);
		~EGLSurfaceWrapper()noexcept;
	} eglSurface;
	
	struct EGLContextWrapper{
		EGLDisplayWrapper& d;
		EGLContext c;
		EGLContextWrapper(EGLDisplayWrapper& d, EGLConfigWrapper& config, EGLSurfaceWrapper& s);
		~EGLContextWrapper()noexcept;
	} eglContext;
#	else
	struct XDisplayWrapper{
		Display* d;
		XDisplayWrapper();
		~XDisplayWrapper()noexcept;
	} xDisplay;

	struct DotsPerInchWrapper{
		float value;

		DotsPerInchWrapper(XDisplayWrapper& display);
	} dotsPerInch_var;

	struct XVisualInfoWrapper{
		XVisualInfo *vi;
		XVisualInfoWrapper(const WindowParams& wp, XDisplayWrapper& xDisplay);
		~XVisualInfoWrapper()noexcept;
	} xVisualInfo;

	struct XWindowWrapper{
		::Window w;

		XDisplayWrapper& d;

		XWindowWrapper(const App::WindowParams& wp, XDisplayWrapper& xDisplay, XVisualInfoWrapper& xVisualInfo);
		~XWindowWrapper()noexcept;
	} xWindow;

	struct GLXContextWrapper{
		GLXContext glxContext;

		XDisplayWrapper& d;
		XWindowWrapper& w;

		GLXContextWrapper(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow, XVisualInfoWrapper& xVisualInfo);
		~GLXContextWrapper()noexcept{
			this->Destroy();
		}

		void Destroy()noexcept;
	} glxContex;

	struct XInputMethodWrapper{
		XIM xim;
		XIC xic;

		XDisplayWrapper& d;
		XWindowWrapper& w;

		XInputMethodWrapper(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow);
		~XInputMethodWrapper()noexcept{
			this->Destroy();
		}

		void Destroy()noexcept;
	} xInputMethod;

	friend void Main(int argc, const char** argv);
	void Exec();

#	endif

#elif M_OS == M_OS_WINDOWS

private:
	struct WindowClassWrapper{
		std::string name;

		WindowClassWrapper();
		~WindowClassWrapper()noexcept;
	} windowClass;

	struct WindowWrapper{
		HWND hwnd;

		WindowWrapper(const WindowParams& wp, const WindowClassWrapper& wc);
		~WindowWrapper()noexcept;
	} window;

	struct DeviceContextWrapper{
		const WindowWrapper& w;
		HDC hdc;

		DeviceContextWrapper(const WindowParams& wp, const WindowWrapper& w);
		~DeviceContextWrapper()noexcept{
			this->Destroy();
		}

	private:
		void Destroy()noexcept;
	} deviceContext;

	struct DotsPerInchWrapper{
		float value;

		DotsPerInchWrapper(DeviceContextWrapper& dc);
	} dotsPerInch_var;

	friend void Main(int argc, const char** argv);
	void Exec();
	friend bool HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& lres);

#elif M_OS == M_OS_MACOSX
private:
	struct DotsPerInchWrapper{
		float value;
		
		DotsPerInchWrapper();
	} dotsPerInch_var;
	
	void macosx_SwapFrameBuffers();
	
	void Exec();
	
	friend void Macosx_Main(int argc, const char** argv);
	friend void Macosx_HandleMouseMove(const morda::Vec2r& pos, unsigned id);
	friend void Macosx_HandleMouseButton(bool isDown, const morda::Vec2r& pos, Widget::EMouseButton button, unsigned id);
	friend void Macosx_HandleMouseHover(bool isHovered);
	friend void Macosx_HandleKeyEvent(bool isDown, EKey keyCode);
	friend void Macosx_HandleCharacterInput(const void* nsstring, EKey key);
	friend void Macosx_UpdateWindowRect(const morda::Rectr& r);
	friend void Macosx_SetQuitFlag();
	
#	if M_OS_NAME == M_OS_NAME_IOS
	struct WindowObject{
		void* id;
		WindowObject(const morda::App::WindowParams& wp);
		~WindowObject()noexcept;
	} windowObject;
	
#	else
	struct ApplicationObject{
		void* id;
		ApplicationObject();
		~ApplicationObject()noexcept;
	} applicationObject;

	struct WindowObject{
		void* id;
		WindowObject(const morda::App::WindowParams& wp);
		~WindowObject()noexcept;
	} windowObject;

	struct OpenGLContext{
		void *id;
		OpenGLContext(void* window);
		~OpenGLContext()noexcept{
			this->Destroy();
		}

		void Destroy()noexcept;
	} openGLContext;
#	endif

#else
#	error "unsupported OS"
#endif

private:
	Render renderer; //This should init the rendering


#if M_OS_NAME != M_OS_NAME_ANDROID
private:
	volatile bool quitFlag = false;
#endif


#if M_OS == M_OS_WINDOWS || M_OS == M_OS_MACOSX
public:
	void postToUiThread_ts(std::function<void()>&& f);
#else
public:
	void postToUiThread_ts(std::function<void()>&& f){	
		this->uiQueue.pushMessage(std::move(f));
	}
private:
	nitki::Queue uiQueue;
#endif

private:
	Updateable::Updater updater;

private:
	DefaultShaders shaders_var;

public:
	DefaultShaders& shaders()noexcept{
		return this->shaders_var;
	}

	std::unique_ptr<papki::File> createResourceFileInterface(const std::string& path = std::string())const;

private:
	//this is a viewport rectangle in coordinates that are as follows: x grows right, y grows up.
	morda::Rectr curWinRect = morda::Rectr(0, 0, 0, 0);

public:
	const morda::Rectr& winRect()const noexcept{
		return this->curWinRect;
	}

public:
	class ResMan : public ResourceManager{
		friend class App;
		ResMan();
	} resMan;

	class Inflater : public morda::Inflater{
		friend class App;
		Inflater();
	} inflater;

private:
	std::shared_ptr<morda::Widget> rootWidget; //NOTE: this should go after resMan as it may hold references to some resources, so it should be destroyed first

	void updateWindowRect(const morda::Rectr& rect);

	void render();

	//pos is in usual window coordinates, y goes down.
	morda::Vec2r nativeWindowToRootCoordinates(const kolme::Vec2f& pos)const noexcept{
		return morda::Vec2r(pos.x, this->winRect().d.y - pos.y - 1.0f);
	}

	//pos is in usual window coordinates, y goes down.
	void handleMouseMove(const kolme::Vec2f& pos, unsigned id);

	//pos is in usual window coordinates, y goes down.
	void handleMouseButton(bool isDown, const kolme::Vec2f& pos, Widget::EMouseButton button, unsigned id);

	void handleMouseHover(bool isHovered, unsigned pointerID);

protected:
	App(const WindowParams& requestedWindowParams);

public:

	virtual ~App()noexcept{}

	void setRootWidget(const std::shared_ptr<morda::Widget>& w){
		this->rootWidget = w;

		this->rootWidget->moveTo(morda::Vec2r(0));
		this->rootWidget->resize(this->winRect().d);
	}

	void showVirtualKeyboard()noexcept;

	void hideVirtualKeyboard()noexcept;

private:
	std::weak_ptr<Widget> focusedWidget;

	void setFocusedWidget(const std::shared_ptr<Widget> w);

	//The idea with UnicodeResolver parameter is that we don't want to calculate the unicode unless it is really needed, thus postpone it
	//as much as possible.
	template <class UnicodeResolver> void handleCharacterInput(const UnicodeResolver& unicodeResolver, EKey key){
		if(auto w = this->focusedWidget.lock()){
//			TRACE(<< "HandleCharacterInput(): there is a focused widget" << std::endl)
			if(auto c = dynamic_cast<CharInputWidget*>(w.operator->())){
				c->onCharacterInput(utki::wrapBuf(unicodeResolver.Resolve()), key);
			}
		}
	}

	void handleKeyEvent(bool isDown, EKey keyCode);

public:

	float dotsPerInch()const noexcept{
		return this->dotsPerInch_var.value;
	}

	float dotsPerCm()const noexcept{
		return this->dotsPerInch() / 2.54f;
	}

	void quit()noexcept;

private:
	bool isFullscreen_var = false;

	kolme::Rectu beforeFullScreenWindowRect;

public:
	bool isFullscreen()const noexcept {
		return this->isFullscreen_var;
	}

	void setFullscreen(bool enable);
};



}//~namespace
