#pragma once

#include <memory>

#include <utki/Singleton.hpp>
#include <utki/config.hpp>
#include <utki/Buf.hpp>
#include <utki/Unique.hpp>
#include <utki/Flags.hpp>

#include <papki/File.hpp>

#include <nitki/Thread.hpp>

#include <kolme/Vector2.hpp>

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

#include "widgets/core/Widget.hpp"
#include "widgets/core/CharInputWidget.hpp"

#include "shaders/ColorPosShader.hpp"
#include "shaders/ColorPosTexShader.hpp"
#include "shaders/ClrPosShader.hpp"
#include "shaders/PosTexShader.hpp"
#include "shaders/SimpleGrayscalePosTexShader.hpp"
#include "shaders/SimpleBlurPosTexShader.hpp"



namespace morda{



/**
 * @brief Base singleton class of Application.
 * An apllication should subclass this class and return an instance from the
 * application factory function createApp(), see AppFactory.hpp for details.
 * When instance of this class is created it also creates a window and
 * initializes OpenGL (or OpenGL ES).
 */
class App :
		public utki::IntrusiveSingleton<App>,
		public utki::Unique
{
	friend class utki::IntrusiveSingleton<App>;
	static utki::IntrusiveSingleton<App>::T_Instance instance;

	friend class Render;
	friend class Updateable;
	friend class Widget;
	friend class CharInputWidget;

	nitki::Thread::T_ThreadID uiThreadId = nitki::Thread::getCurrentThreadID();

public:
	/**
	 * @brief Desired window parameters.
	 */
	struct WindowParams{
		/**
		 * @brief Desired dimensions of the window
		 */
		kolme::Vec2ui dim;
		
		enum class Buffer_e{
			DEPTH,
			STENCIL,
			
			ENUM_SIZE
		};
		
		/**
		 * @brief Flags describing desired buffers for OpneGL context.
		 */
		utki::Flags<Buffer_e> buffers = utki::Flags<Buffer_e>(false);
		
		WindowParams(kolme::Vec2ui dim) :
				dim(dim)
		{}
	};

private:
	WindowParams windowParams; //this is to save window params
	
public:
	/**
	 * @brief tell if this is the UI thread.
	 * @return true if this is UI thread.
	 * @return false otherwise.
	 */
	bool thisIsUIThread()const noexcept{
		return this->uiThreadId == nitki::Thread::getCurrentThreadID();
	}

	/**
	 * @brief Collection of standard shaders.
	 */
	struct DefaultShaders{
		ColorPosShader colorPosShader;
		ColorPosTexShader colorPosTexShader;
		ClrPosShader clrPosShader;
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

	struct XEmptyMouseCursor{
		Cursor c;
		
		XDisplayWrapper& d;
		
		XEmptyMouseCursor(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow);
		~XEmptyMouseCursor()noexcept;
	} xEmptyMouseCursor;
	
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

		bool isHovered = false; //for tracking when mouse enters or leaves window.

		utki::Flags<Widget::MouseButton_e> mouseButtonState;

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

	bool mouseCursorIsCurrentlyVisible = true;
	
	friend void Main(int argc, const char** argv);
	void Exec();
	friend bool HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& lres);

#elif M_OS == M_OS_MACOSX
private:
	
#	if M_OS_NAME == M_OS_NAME_IOS
	struct WindowObject{
		void* id;
		WindowObject(const morda::App::WindowParams& wp);
		~WindowObject()noexcept;
	} windowObject;
	
	friend void ios_render();
	friend std::uint32_t ios_update();
	friend void ios_updateWindowRect(morda::Vec2r dim);
	friend void ios_handleMouseMove(const morda::Vec2r& pos, unsigned id);
	friend void ios_handleMouseButton(bool isDown, const morda::Vec2r& pos, morda::Widget::MouseButton_e button, unsigned id);
	friend const morda::App::WindowParams& ios_getWindowParams();
	
#	else
	void macosx_SwapFrameBuffers();
	
	void Exec();
	
	friend void Macosx_Main(int argc, const char** argv);
	friend void Macosx_HandleMouseMove(const morda::Vec2r& pos, unsigned id);
	friend void Macosx_HandleMouseButton(bool isDown, const morda::Vec2r& pos, Widget::MouseButton_e button, unsigned id);
	friend void Macosx_HandleMouseHover(bool isHovered);
	friend void Macosx_HandleKeyEvent(bool isDown, Key_e keyCode);
	friend void Macosx_HandleCharacterInput(const void* nsstring, Key_e key);
	friend void Macosx_UpdateWindowRect(const morda::Rectr& r);
	friend void Macosx_SetQuitFlag();

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
		OpenGLContext(const morda::App::WindowParams& wp, void* window);
		~OpenGLContext()noexcept{
			this->Destroy();
		}

		void Destroy()noexcept;
	} openGLContext;
	
	bool mouseCursorIsCurrentlyVisible = true;
	
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
	/**
	 * @brief Execute function on UI thread.
	 * This function is thread safe. It posts the function to the queue of execution on UI thread,
	 * the function will be executed on next UI cycle.
	 * @param f - function to execute on UI thread.
	 */
	void postToUiThread_ts(std::function<void()>&& f){	
		this->uiQueue.pushMessage(std::move(f));
	}
private:
	nitki::Queue uiQueue;
#endif

private:
	Updateable::Updater updater;

public:
	/**
	 * @brief Standard shaders.
	 * This is the instantiation of morda's standard shaders available for use.
	 */
	DefaultShaders shaders;

	/**
	 * @brief Create file interface into resources storage.
	 * This function creates a morda's standard file interface to read application's
	 * recources.
	 * @param path - file path to initialize the file interface with.
	 * @return Instance of the file interface into the resources storage.
	 */
	std::unique_ptr<papki::File> createResourceFileInterface(const std::string& path = std::string())const;

private:
	//this is a viewport rectangle in coordinates that are as follows: x grows right, y grows up.
	morda::Rectr curWinRect = morda::Rectr(0, 0, 0, 0);

public:
	/**
	 * @brief Get current window rectangle.
	 * @return Current application window rectangle.
	 */
	const morda::Rectr& winRect()const noexcept{
		return this->curWinRect;
	}

public:
	/**
	 * @brief Instantiation of the resource manager.
	 */
	ResourceManager resMan;

	/**
	 * @brief Instantiation of the GUI inflater.
	 */
	Inflater inflater;

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
	void handleMouseButton(bool isDown, const kolme::Vec2f& pos, Widget::MouseButton_e button, unsigned id);

	void handleMouseHover(bool isHovered, unsigned pointerID);

protected:
	/**
	 * @brief Application constructor.
	 * @param requestedWindowParams - requested window parameters.
	 */
	App(const WindowParams& requestedWindowParams);

public:

	virtual ~App()noexcept{}

	/**
	 * @brief Set the root widget of the application.
	 * @param w - the widget to set as a root widget.
	 */
	void setRootWidget(const std::shared_ptr<morda::Widget>& w){
		this->rootWidget = w;

		this->rootWidget->moveTo(morda::Vec2r(0));
		this->rootWidget->resize(this->winRect().d);
	}

	/**
	 * @brief Bring up the virtual keyboard.
	 * On mobile platforms this function will summon the on-screen keyboard.
	 * On desktop platforms this function does nothing.
	 */
	void showVirtualKeyboard()noexcept;

	/**
	 * @brief Hide virtual keyboard.
	 * On mobile platforms this function hides the on-screen keyboard.
	 * On desktop platforms this function does nothing.
	 */
	void hideVirtualKeyboard()noexcept;

private:
	std::weak_ptr<Widget> focusedWidget;

	void setFocusedWidget(const std::shared_ptr<Widget> w);

	//The idea with UnicodeResolver parameter is that we don't want to calculate the unicode unless it is really needed, thus postpone it
	//as much as possible.
	template <class UnicodeResolver> void handleCharacterInput(const UnicodeResolver& unicodeResolver, Key_e key){
		if(auto w = this->focusedWidget.lock()){
//			TRACE(<< "HandleCharacterInput(): there is a focused widget" << std::endl)
			if(auto c = dynamic_cast<CharInputWidget*>(w.operator->())){
				c->onCharacterInput(utki::wrapBuf(unicodeResolver.Resolve()), key);
			}
		}
	}

	void handleKeyEvent(bool isDown, Key_e keyCode);

public:
	
	/**
	 * @brief Information about screen units.
	 * This class holds information about screen units and performs conversion
	 * from one unit to another.
	 * In morda, length can be expressed in pixels, millimeters or points.
	 * Points is a convenience unit which is different depending on the screen dimensions
	 * and density. Point is never less than one pixel.
	 * For normal desktop displays like HP or Full HD point is equal to one pixel.
	 * For higher density desktop displays point is more than one pixel depending on density.
	 * For mobile platforms the point is also 1 or more pixels depending on display density and physical size.
	 */
	class Units{
		real dotsPerInch_v;
		real dotsPerPt_v;
	public:
		/**
		 * @brief Constructor.
		 * @param dotsPerInch - dots per inch.
		 * @param dotsPerPt - dots per point.
		 */
		Units(real dotsPerInch, real dotsPerPt) :
				dotsPerInch_v(dotsPerInch),
				dotsPerPt_v(dotsPerPt)
		{}
		
		/**
		 * @brief Get dots (pixels) per inch.
		 * @return Dots per inch.
		 */
		real dpi()const noexcept{
			return this->dotsPerInch_v;
		}
		
		/**
		 * @brief Get dots (pixels) per centimeter.
		 * @return Dots per centimeter.
		 */
		real dotsPerCm()const noexcept{
			return this->dpi() / 2.54f;
		}
		
		/**
		 * @brief Get dots (pixels) per point.
		 * @return Dots per point.
		 */
		real dotsPerPt()const noexcept{
			return this->dotsPerPt_v;
		}
		
		/**
		 * @brief Convert millimeters to pixels (dots).
		 * @param mm - value in millimeters.
		 * @return Value in pixels.
		 */
		real mmToPx(real mm)const noexcept{
			return std::round(mm * this->dotsPerCm() / 10.0f);
		}
		
		/**
		 * @brief Convert points to pixels.
		 * @param pt - value in points.
		 * @return  Value in pixels.
		 */
		real ptToPx(real pt)const noexcept{
			return std::round(pt * this->dotsPerPt());
		}
	} units;
	
	/**
	 * @brief Requests application to exit.
	 * This function posts an exit message to the applications message queue.
	 * The message will normally be handled on the next UI cycle and as a result
	 * the main loop will be terminated and application will exit. The Application
	 * object will be destroyed and all resources freed.
	 */
	void quit()noexcept;

private:
	bool isFullscreen_v = false;

	kolme::Rectu beforeFullScreenWindowRect;

public:
	/**
	 * @brief Check if application currently runs in fullscreen mode.
	 * @return true if application is in fullscreen mode.
	 * @return false if application is in windowed mode.
	 */
	bool isFullscreen()const noexcept {
		return this->isFullscreen_v;
	}

	/**
	 * @brief Set/unset fullscreen mode.
	 * @param enable - whether to enable or to disable fullscreen mode.
	 */
	void setFullscreen(bool enable);
	
	/**
	 * @brief Initialize standard widgets library.
	 * In addition to core widgets it is possible to use standard widgets.
	 * This function loads necessarey resource packs and initializes standard
	 * widgets to be used by application.
	 */
	void initStandardWidgets();
	

	/**
	 * @brief Show/hide mouse cursor.
	 * @param visible - whether to show (true) or hide (false) mouse cursor.
	 */
	void setMouseCursorVisible(bool visible);
};



#if M_OS == M_OS_LINUX || M_OS == M_OS_MACOSX || M_OS == M_OS_UNIX
/**
 * @brief For internal use only.
 */
std::unique_ptr<App> createAppUnix(int argc, const char** argv, const utki::Buf<std::uint8_t> savedState);
#endif



}
