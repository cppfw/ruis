#include <vector>
#include <array>

#include "../../application.hpp"

#include <opros/wait_set.hpp>
#include <papki/fs_file.hpp>
#include <nitki/queue.hpp>

#include <utki/unicode.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#ifdef MORDAVOKNE_RENDER_OPENGL2
#	include <GL/glew.h>
#	include <GL/glx.h>

#	include <morda/render/opengl2/renderer.hpp>

#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
#	include <EGL/egl.h>
#	ifdef M_RASPBERRYPI
#		include <bcm_host.h>
#	endif

#	include <morda/render/opengles2/renderer.hpp>

#else
#	error "Unknown graphics API"
#endif

#include "../friendAccessors.cxx"
#include "../unixCommon.cxx"

using namespace mordavokne;

namespace{
struct WindowWrapper : public utki::destructable{
	Display* display;
	Colormap colorMap;
	::Window window;
#ifdef MORDAVOKNE_RENDER_OPENGL2
	GLXContext glContext;
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
#	ifdef M_RASPBERRYPI
	EGL_DISPMANX_WINDOW_T rpiNativeWindow;
	DISPMANX_DISPLAY_HANDLE_T rpiDispmanDisplay;
	DISPMANX_UPDATE_HANDLE_T rpiDispmanUpdate;
	DISPMANX_ELEMENT_HANDLE_T rpiDispmanElement;
#	endif
	EGLDisplay eglDisplay;
	EGLSurface eglSurface;
	EGLContext eglContext;
#else
#	error "Unknown graphics API"
#endif
	Cursor emptyCursor;
	XIM inputMethod;
	XIC inputContext;

	nitki::queue ui_queue;

	volatile bool quitFlag = false;

	WindowWrapper(const window_params& wp){
		this->display = XOpenDisplay(0);
		if(!this->display){
			throw std::runtime_error("XOpenDisplay() failed");
		}
		utki::scope_exit scopeExitDisplay([this](){
			XCloseDisplay(this->display);
		});

#ifdef MORDAVOKNE_RENDER_OPENGL2
		{
			int glxVerMajor, glxVerMinor;
			if(!glXQueryVersion(this->display, &glxVerMajor, &glxVerMinor)){
				throw std::runtime_error("glXQueryVersion() failed");
			}

			// FBConfigs were added in GLX version 1.3.
			if(glxVerMajor < 1 || (glxVerMajor == 1  && glxVerMinor < 3 )){
				throw std::runtime_error("GLX version 1.3 or above is required");
			}
		}

		GLXFBConfig bestFbc;
		{
			std::vector<int> visualAttribs;
			visualAttribs.push_back(GLX_X_RENDERABLE); visualAttribs.push_back(True);
			visualAttribs.push_back(GLX_X_VISUAL_TYPE); visualAttribs.push_back(GLX_TRUE_COLOR);
			visualAttribs.push_back(GLX_DRAWABLE_TYPE); visualAttribs.push_back(GLX_WINDOW_BIT);
			visualAttribs.push_back(GLX_RENDER_TYPE); visualAttribs.push_back(GLX_RGBA_BIT);
			visualAttribs.push_back(GLX_DOUBLEBUFFER);visualAttribs.push_back(True);
			visualAttribs.push_back(GLX_RED_SIZE); visualAttribs.push_back(8);
			visualAttribs.push_back(GLX_GREEN_SIZE); visualAttribs.push_back(8);
			visualAttribs.push_back(GLX_BLUE_SIZE); visualAttribs.push_back(8);
			visualAttribs.push_back(GLX_ALPHA_SIZE); visualAttribs.push_back(8);

			if(wp.buffers.get(window_params::buffer_type::depth)){
				visualAttribs.push_back(GLX_DEPTH_SIZE); visualAttribs.push_back(24);
			}
			if(wp.buffers.get(window_params::buffer_type::stencil)){
				visualAttribs.push_back(GLX_STENCIL_SIZE); visualAttribs.push_back(8);
			}

			visualAttribs.push_back(None);

			int fbcount;
			GLXFBConfig* fbc = glXChooseFBConfig(this->display, DefaultScreen(this->display), &*visualAttribs.begin(), &fbcount);
			if(!fbc){
				throw std::runtime_error("glXChooseFBConfig() returned empty list");
			}
			utki::scope_exit scopeExitFbc([&fbc](){
				XFree(fbc);
			});

			int bestFbcIdx = -1, worstFbc = -1, bestNumSamp = -1, worstNumSamp = 999;

			for(int i = 0; i < fbcount; ++i){
				XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
				if(vi){
					int samp_buf, samples;
					glXGetFBConfigAttrib(this->display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
					glXGetFBConfigAttrib(this->display, fbc[i], GLX_SAMPLES, &samples);

					if ( bestFbcIdx < 0 || (samp_buf && samples > bestNumSamp )){
						bestFbcIdx = i, bestNumSamp = samples;
					}
					if ( worstFbc < 0 || !samp_buf || samples < worstNumSamp ){
						worstFbc = i, worstNumSamp = samples;
					}
				}
				XFree( vi );
			}
			bestFbc = fbc[ bestFbcIdx ];
		}
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
		this->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		if(this->eglDisplay == EGL_NO_DISPLAY){
			throw std::runtime_error("eglGetDisplay(): failed, no matching display connection found");
		}

		utki::scope_exit scopeExitEGLDisplay([this](){
			eglTerminate(this->eglDisplay);
		});

		if(eglInitialize(this->eglDisplay, nullptr, nullptr) == EGL_FALSE){
			eglTerminate(this->eglDisplay);
			throw std::runtime_error("eglInitialize() failed");
		}

		EGLConfig eglConfig;
		{
			// TODO: allow stencil and depth configuration etc. via window_params
			// Here specify the attributes of the desired configuration.
			// Below, we select an EGLConfig with at least 8 bits per color
			// component compatible with on-screen windows.
			const EGLint attribs[] = {
					EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
					EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, //we want OpenGL ES 2.0
					EGL_BLUE_SIZE, 8,
					EGL_GREEN_SIZE, 8,
					EGL_RED_SIZE, 8,
					EGL_ALPHA_SIZE, 8,
					EGL_DEPTH_SIZE, 16,
					EGL_NONE
			};

			// Here, the application chooses the configuration it desires. In this
			// sample, we have a very simplified selection process, where we pick
			// the first EGLConfig that matches our criteria.
			EGLint numConfigs;
			eglChooseConfig(this->eglDisplay, attribs, &eglConfig, 1, &numConfigs);
			if(numConfigs <= 0){
				throw std::runtime_error("eglChooseConfig() failed, no matching config found");
			}
		}

		if(eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE){
			throw std::runtime_error("eglBindApi() failed");
		}
#else
#	error "Unknown graphics API"
#endif

		XVisualInfo *vi;
#ifdef MORDAVOKNE_RENDER_OPENGL2
		vi = glXGetVisualFromFBConfig(this->display, bestFbc);
		if (!vi) {
			throw std::runtime_error("glXGetVisualFromFBConfig() failed");
		}
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
#	ifdef M_RASPBERRYPI
		{
			int numVisuals;
			XVisualInfo visTemplate;
			visTemplate.screen = DefaultScreen(this->display); //LCD
			vi = XGetVisualInfo(
					this->display,
					VisualScreenMask,
					&visTemplate,
					&numVisuals
				);
			if (!vi) {
				throw std::runtime_error("XGetVisualInfo() failed");
			}
		}
#	else
		{
			EGLint vid;

			if(!eglGetConfigAttrib(this->eglDisplay, eglConfig, EGL_NATIVE_VISUAL_ID, &vid)) {
				throw std::runtime_error("eglGetConfigAttrib() failed");
			}

			int numVisuals;
			XVisualInfo visTemplate;
			visTemplate.visualid = vid;
			vi = XGetVisualInfo(
					this->display,
					VisualIDMask,
					&visTemplate,
					&numVisuals
				);
			if (!vi) {
				throw std::runtime_error("XGetVisualInfo() failed");
			}
		}
#	endif
#else
#	error "Unknown graphics API"
#endif
		utki::scope_exit scopeExitVisualInfo([vi](){
			XFree(vi);
		});

		this->colorMap = XCreateColormap(
				this->display,
				RootWindow(this->display, vi->screen),
				vi->visual,
				AllocNone
			);
		//TODO: check for error?
		utki::scope_exit scopeExitColorMap([this](){
			XFreeColormap(this->display, this->colorMap);
		});

		{
			XSetWindowAttributes attr;
			attr.colormap = colorMap;
			attr.border_pixel = 0;
			attr.background_pixmap = None;
			attr.event_mask =
					ExposureMask |
					KeyPressMask |
					KeyReleaseMask |
					ButtonPressMask |
					ButtonReleaseMask |
					PointerMotionMask |
					ButtonMotionMask |
					StructureNotifyMask |
					EnterWindowMask |
					LeaveWindowMask
				;
			unsigned long fields = CWBorderPixel | CWColormap | CWEventMask;

			this->window = XCreateWindow(
					this->display,
					RootWindow(this->display, vi->screen),
					0,
					0,
					wp.dim.x(),
					wp.dim.y(),
					0,
					vi->depth,
					InputOutput,
					vi->visual,
					fields,
					&attr
				);
		}
		if(!this->window){
			throw std::runtime_error("Failed to create window");
		}
		utki::scope_exit scopeExitWindow([this](){
			XDestroyWindow(this->display, this->window);
		});

		{//We want to handle WM_DELETE_WINDOW event to know when window is closed.
			Atom a = XInternAtom(this->display, "WM_DELETE_WINDOW", True);
			XSetWMProtocols(this->display, this->window, &a, 1);
		}

		XMapWindow(this->display, this->window);

		XFlush(this->display);

#ifdef MORDAVOKNE_RENDER_OPENGL2
		this->glContext = glXCreateContext(this->display, vi, 0, GL_TRUE);
		if(this->glContext == NULL){
			throw std::runtime_error("glXCreateContext() failed");
		}
		utki::scope_exit scopeExitGLContext([this](){
			glXMakeCurrent(this->display, None, NULL);
			glXDestroyContext(this->display, this->glContext);
		});

		glXMakeCurrent(this->display, this->window, this->glContext);

		TRACE(<< "OpenGL version: " << glGetString(GL_VERSION) << std::endl)

		if(glewInit() != GLEW_OK){
			throw std::runtime_error("GLEW initialization failed");
		}
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)

#	ifdef M_RASPBERRYPI
		{
			bcm_host_init();

			VC_RECT_T dst_rect, src_rect;

			uint32_t display_width, display_height;

			// create an EGL window surface, passing context width/height
			if(graphics_get_display_size(
					0, //LCD
					&display_width,
					&display_height
				) < 0 )
			{
				throw std::runtime_error("graphics_get_display_size() failed");
			}

			dst_rect.x = 0;
			dst_rect.y = 0;
			dst_rect.width = display_width;
			dst_rect.height = display_height;

			src_rect.x = 0;
			src_rect.y = 0;
			src_rect.width = display_width << 16;
			src_rect.height = display_height << 16;

			this->rpiDispmanDisplay = vc_dispmanx_display_open(0); //0 = LCD
			this->rpiDispmanUpdate = vc_dispmanx_update_start(0);

			this->rpiDispmanElement = vc_dispmanx_element_add(
					this->rpiDispmanUpdate,
					this->rpiDispmanDisplay,
					0, // layer
					&dst_rect,
					0, // src
					&src_rect,
					DISPMANX_PROTECTION_NONE,
					0, // alpha
					0, // clamp
					DISPMANX_NO_ROTATE // transform
				);

			this->rpiNativeWindow.element = this->rpiDispmanElement;
			this->rpiNativeWindow.width = display_width;
			this->rpiNativeWindow.height = display_height;
			vc_dispmanx_update_submit_sync(this->rpiDispmanUpdate);
		}
#	endif

		this->eglSurface = eglCreateWindowSurface(
				this->eglDisplay,
				eglConfig,
#	ifdef M_RASPBERRYPI
				reinterpret_cast<EGLNativeWindowType>(&this->rpiNativeWindow),
#	else
				this->window,
#	endif
				nullptr
			);
		if(this->eglSurface == EGL_NO_SURFACE){
			throw std::runtime_error("eglCreateWindowSurface() failed");
		}
		utki::scope_exit scopeExitEGLSurface([this](){
			eglDestroySurface(this->eglDisplay, this->eglSurface);
		});

		{
			EGLint contextAttrs[] = {
				EGL_CONTEXT_CLIENT_VERSION, 2, // this is needed at least on Android, otherwise eglCreateContext() thinks that we want OpenGL ES 1.1, but we want 2.0
				EGL_NONE
			};

			this->eglContext = eglCreateContext(this->eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttrs);
			if(this->eglContext == EGL_NO_CONTEXT){
				throw std::runtime_error("eglCreateContext() failed");
			}
		}

		if(eglMakeCurrent(this->eglDisplay, this->eglSurface, this->eglSurface, this->eglContext) == EGL_FALSE){
			eglDestroyContext(this->eglDisplay, this->eglContext);
			throw std::runtime_error("eglMakeCurrent() failed");
		}
		utki::scope_exit scopeExitEGLContext([this](){
			eglMakeCurrent(this->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			eglDestroyContext(this->eglDisplay, this->eglContext);
		});
#else
#	error "Unknown graphics API"
#endif

		{
			Pixmap blank;
			XColor dummy;
			char data[1] = {0};

			blank = XCreateBitmapFromData(this->display, this->window, data, 1, 1);
			if(blank == None){
				throw std::runtime_error("application::XEmptyMouseCursor::XEmptyMouseCursor(): could not create bitmap");
			}
			utki::scope_exit scopeExit([this, &blank](){
				XFreePixmap(this->display, blank);
			});

			this->emptyCursor = XCreatePixmapCursor(this->display, blank, blank, &dummy, &dummy, 0, 0);
		}
		utki::scope_exit scopeExitEmptyCursor([this](){
			XFreeCursor(this->display, this->emptyCursor);
		});

		this->inputMethod = XOpenIM(this->display, NULL, NULL, NULL);
		if(this->inputMethod == NULL){
			throw std::runtime_error("XOpenIM() failed");
		}
		utki::scope_exit scopeExitInputMethod([this](){
			XCloseIM(this->inputMethod);
		});

		this->inputContext = XCreateIC(
				this->inputMethod,
				XNClientWindow, this->window,
				XNFocusWindow, this->window,
				XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
				NULL
			);
		if(this->inputContext == NULL){
			throw std::runtime_error("XCreateIC() failed");
		}
		utki::scope_exit scopeExitInputContext([this](){
			XUnsetICFocus(this->inputContext);
			XDestroyIC(this->inputContext);
		});

		scopeExitInputContext.reset();
		scopeExitInputMethod.reset();
		scopeExitEmptyCursor.reset();
		scopeExitWindow.reset();
		scopeExitColorMap.reset();
		scopeExitDisplay.reset();
#ifdef MORDAVOKNE_RENDER_OPENGL2
		scopeExitGLContext.reset();
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
		scopeExitEGLDisplay.reset();
		scopeExitEGLSurface.reset();
		scopeExitEGLContext.reset();
#else
#	error "Unknown graphics API"
#endif
	}
	~WindowWrapper()noexcept{
		XUnsetICFocus(this->inputContext);
		XDestroyIC(this->inputContext);

		XCloseIM(this->inputMethod);
		XFreeCursor(this->display, this->emptyCursor);

#ifdef MORDAVOKNE_RENDER_OPENGL2
		glXMakeCurrent(this->display, None, NULL);
		glXDestroyContext(this->display, this->glContext);
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
		eglMakeCurrent(this->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(this->eglDisplay, this->eglContext);
		eglDestroySurface(this->eglDisplay, this->eglSurface);
#else
#	error "Unknown graphics API"
#endif

		XDestroyWindow(this->display, this->window);
		XFreeColormap(this->display, this->colorMap);

#ifdef MORDAVOKNE_RENDER_OPENGLES2
		eglTerminate(this->eglDisplay);
#endif

		XCloseDisplay(this->display);
	}
};

WindowWrapper& getImpl(const std::unique_ptr<utki::destructable>& pimpl){
	ASSERT(dynamic_cast<WindowWrapper*>(pimpl.get()))
	return static_cast<WindowWrapper&>(*pimpl);
}

}

namespace{
morda::real getDotsPerInch(Display* display){
	int scrNum = 0;
	morda::real value = ((morda::real(DisplayWidth(display, scrNum)) / (morda::real(DisplayWidthMM(display, scrNum))/ 10.0))
			+ (morda::real(DisplayHeight(display, scrNum)) / (morda::real(DisplayHeightMM(display, scrNum)) / 10.0))) / 2;
	value *= 2.54f;
	return value;
}

morda::real getDotsPerPt(Display* display){
	int scrNum = 0;
	r4::vector2<unsigned> resolution(DisplayWidth(display, scrNum), DisplayHeight(display, scrNum));
	r4::vector2<unsigned> screenSizeMm(DisplayWidthMM(display, scrNum), DisplayHeightMM(display, scrNum));

	return application::get_pixels_per_dp(resolution, screenSizeMm);
}
}

application::application(std::string&& name, const window_params& requestedWindowParams) :
		name(name),
		windowPimpl(std::make_unique<WindowWrapper>(requestedWindowParams)),
		gui(
#ifdef MORDAVOKNE_RENDER_OPENGL2
				std::make_shared<morda::render_opengl2::renderer>(),
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
				std::make_shared<morda::render_opengles2::renderer>(),
#else
#	error "Unknown graphics API"
#endif
				std::make_shared<morda::updater>(),
				[this](std::function<void()>&& a){
					getImpl(getWindowPimpl(*this)).ui_queue.push_back(std::move(a));
				},
				getDotsPerInch(getImpl(windowPimpl).display),
				::getDotsPerPt(getImpl(windowPimpl).display)
			),
		storage_dir(initializeStorageDir(this->name))
{
#ifdef M_RASPBERRYPI
	this->set_fullscreen(true);
#endif
}

namespace{

class XEvent_waitable : public opros::waitable{
public:
	int fd;

	int get_handle() override{
		return this->fd;
	}

	XEvent_waitable(Display* d){
		this->fd = XConnectionNumber(d);
	}

	void clear_read_flag(){
		this->readiness_flags.clear(opros::ready::read);
	}
};

morda::mouse_button buttonNumberToEnum(int number){
	switch(number){
		case 1:
			return morda::mouse_button::left;
		default:
		case 2:
			return morda::mouse_button::middle;
		case 3:
			return morda::mouse_button::right;
		case 4:
			return morda::mouse_button::wheel_up;
		case 5:
			return morda::mouse_button::wheel_down;
	}
}

const std::array<morda::key, std::uint8_t(-1) + 1> keyCodeMap = {{
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::escape, // 9
	morda::key::one, // 10
	morda::key::two, // 11
	morda::key::three, // 12
	morda::key::four, // 13
	morda::key::five, // 14
	morda::key::six, // 15
	morda::key::seven, // 16
	morda::key::eight, // 17
	morda::key::nine, // 18
	morda::key::zero, // 19
	morda::key::minus, // 20
	morda::key::equals, // 21
	morda::key::backspace, // 22
	morda::key::tabulator, // 23
	morda::key::q, // 24
	morda::key::w, // 25
	morda::key::e, // 26
	morda::key::r, // 27
	morda::key::t, // 28
	morda::key::y, // 29
	morda::key::u, // 30
	morda::key::i, // 31
	morda::key::o, // 32
	morda::key::p, // 33
	morda::key::left_square_bracket, // 34
	morda::key::right_square_bracket, // 35
	morda::key::enter, // 36
	morda::key::left_control, // 37
	morda::key::a, // 38
	morda::key::s, // 39
	morda::key::d, // 40
	morda::key::f, // 41
	morda::key::g, // 42
	morda::key::h, // 43
	morda::key::j, // 44
	morda::key::k, // 45
	morda::key::l, // 46
	morda::key::semicolon, // 47
	morda::key::apostrophe, // 48
	morda::key::grave, // 49
	morda::key::left_shift, // 50
	morda::key::backslash, // 51
	morda::key::z, // 52
	morda::key::x, // 53
	morda::key::c, // 54
	morda::key::v, // 55
	morda::key::b, // 56
	morda::key::n, // 57
	morda::key::m, // 58
	morda::key::comma, // 59
	morda::key::period, // 60
	morda::key::slash, // 61
	morda::key::right_shift, // 62
	morda::key::unknown,
	morda::key::left_alt, // 64
	morda::key::space, // 65
	morda::key::capslock, // 66
	morda::key::f1, // 67
	morda::key::f2, // 68
	morda::key::f3, // 69
	morda::key::f4, // 70
	morda::key::f5, // 71
	morda::key::f6, // 72
	morda::key::f7, // 73
	morda::key::f8, // 74
	morda::key::f9, // 75
	morda::key::f10, // 76
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::f11, // 95
	morda::key::f12, // 96
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::right_control, // 105
	morda::key::unknown,
	morda::key::print_screen, // 107
	morda::key::right_alt, // 108
	morda::key::unknown,
	morda::key::home, // 110
	morda::key::up, // 111
	morda::key::page_up, // 112
	morda::key::left, // 113
	morda::key::right, // 114
	morda::key::end, // 115
	morda::key::down, // 116
	morda::key::page_down, // 117
	morda::key::insert, // 118
	morda::key::deletion, // 119
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::pause, // 127
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::command, // 133
	morda::key::unknown,
	morda::key::menu, // 135
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown,
	morda::key::unknown
}};

class KeyEventUnicodeProvider : public morda::gui::unicode_provider{
	XIC& xic;
	XEvent& event;
public:
	KeyEventUnicodeProvider(XIC& xic, XEvent& event) :
			xic(xic),
			event(event)
	{}

	std::u32string get()const override{
#ifndef X_HAVE_UTF8_STRING
#	error "no Xutf8stringlookup()"
#endif

		Status status;

		std::array<char, 32> staticBuf;
		std::vector<char> arr;
		auto buf = utki::make_span(staticBuf);

		int size = Xutf8LookupString(this->xic, &this->event.xkey, buf.begin(), buf.size() - 1, NULL, &status);
		if(status == XBufferOverflow){
			//allocate enough memory
			arr.resize(size + 1);
			buf = utki::make_span(arr);
			size = Xutf8LookupString(this->xic, &this->event.xkey, buf.begin(), buf.size() - 1, NULL, &status);
		}
		ASSERT(size >= 0)
		ASSERT(buf.size() != 0)
		ASSERT(buf.size() > unsigned(size))

//		TRACE(<< "KeyEventUnicodeResolver::Resolve(): size = " << size << std::endl)

		buf[size] = 0;//null-terminate

		switch(status){
			case XLookupChars:
			case XLookupBoth:
				if(size == 0){
					return std::u32string();
				}
				return utki::to_utf32(&*buf.begin());
			default:
			case XBufferOverflow:
				ASSERT(false)
			case XLookupKeySym:
			case XLookupNone:
				break;
		}

		return std::u32string();
	}
};

}

void application::quit()noexcept{
	auto& ww = getImpl(this->windowPimpl);

	ww.quitFlag = true;
}

int main(int argc, const char** argv){
	std::unique_ptr<mordavokne::application> app = createAppUnix(argc, argv);

	ASSERT(app)

	auto& ww = getImpl(getWindowPimpl(*app));

	XEvent_waitable xew(ww.display);

	opros::wait_set wait_set(2);

	wait_set.add(xew, {opros::ready::read});
	wait_set.add(ww.ui_queue, {opros::ready::read});

	// Sometimes the first Expose event does not come for some reason. It happens constantly in some systems and never happens on all the others.
	// So, render everything for the first time.
	render(*app);

	while(!ww.quitFlag){
		xew.clear_read_flag(); // clear read flag because we have no 'read' function in XEvent_waitable which would do that for us

		auto num_waitables_triggered = wait_set.wait(app->gui.update());
		// TRACE(<< "num_waitables_triggered = " << num_waitables_triggered << std::endl)

		bool ui_queue_ready_to_read = ww.ui_queue.flags().get(opros::ready::read);
		if(ui_queue_ready_to_read){
			while(auto m = ww.ui_queue.pop_front()){
				TRACE(<< "loop message" << std::endl)
				m();
			}
			ASSERT(!ww.ui_queue.flags().get(opros::ready::read))
		}

		// NOTE: do not check 'read' flag for X event, for some reason when waiting with 0 timeout it will never be set.
		//       Maybe some bug in XWindows, maybe something else.
		bool x_event_arrived = false;
		while(XPending(ww.display) > 0){
			x_event_arrived = true;
			XEvent event;
			XNextEvent(ww.display, &event);
			// TRACE(<< "X event got, type = " << (event.type) << std::endl)
			switch(event.type){
				case Expose:
//						TRACE(<< "Expose X event got" << std::endl)
					if(event.xexpose.count != 0){
						break;
					}
					render(*app);
					break;
				case ConfigureNotify:
//						TRACE(<< "ConfigureNotify X event got" << std::endl)
					updateWindowRect(*app, morda::rectangle(0, 0, float(event.xconfigure.width), float(event.xconfigure.height)));
					break;
				case KeyPress:
//						TRACE(<< "KeyPress X event got" << std::endl)
					{
						morda::key key = keyCodeMap[std::uint8_t(event.xkey.keycode)];
						handleKeyEvent(*app, true, key);
						handleCharacterInput(*app, KeyEventUnicodeProvider(ww.inputContext, event), key);
					}
					break;
				case KeyRelease:
//						TRACE(<< "KeyRelease X event got" << std::endl)
					{
						morda::key key = keyCodeMap[std::uint8_t(event.xkey.keycode)];

						// detect auto-repeated key events
						if(XEventsQueued(ww.display, QueuedAfterReading)){ // if there are other events queued
							XEvent nev;
							XPeekEvent(ww.display, &nev);

							if(nev.type == KeyPress
									&& nev.xkey.time == event.xkey.time
									&& nev.xkey.keycode == event.xkey.keycode
								)
							{
								// key wasn't actually released
								handleCharacterInput(*app, KeyEventUnicodeProvider(ww.inputContext, nev), key);

								XNextEvent(ww.display, &nev); // remove the key down event from queue
								break;
							}
						}

						handleKeyEvent(*app, false, key);
					}
					break;
				case ButtonPress:
//						TRACE(<< "ButtonPress X event got, button mask = " << event.xbutton.button << std::endl)
//						TRACE(<< "ButtonPress X event got, x, y = " << event.xbutton.x << ", " << event.xbutton.y << std::endl)
					handleMouseButton(
							*app,
							true,
							morda::vector2(event.xbutton.x, event.xbutton.y),
							buttonNumberToEnum(event.xbutton.button),
							0
						);
					break;
				case ButtonRelease:
//						TRACE(<< "ButtonRelease X event got, button mask = " << event.xbutton.button << std::endl)
					handleMouseButton(
							*app,
							false,
							morda::vector2(event.xbutton.x, event.xbutton.y),
							buttonNumberToEnum(event.xbutton.button),
							0
						);
					break;
				case MotionNotify:
//						TRACE(<< "MotionNotify X event got" << std::endl)
					handleMouseMove(
							*app,
							morda::vector2(event.xmotion.x, event.xmotion.y),
							0
						);
					break;
				case EnterNotify:
					handleMouseHover(*app, true, 0);
					break;
				case LeaveNotify:
					handleMouseHover(*app, false, 0);
					break;
				case ClientMessage:
//						TRACE(<< "ClientMessage X event got" << std::endl)
					// probably a WM_DELETE_WINDOW event
					{
						char* name = XGetAtomName(ww.display, event.xclient.message_type);
						if(*name == *"WM_PROTOCOLS"){
							ww.quitFlag = true;
						}
						XFree(name);
					}
					break;
				default:
					// ignore
					break;
			}
		}

		// WORKAROUND: XEvent file descriptor becomes ready to read many times per second, even if
		//             there are no events to handle returned by XPending(), so here we check if something
		//             meaningful actuall happened and call render() only if it did
		if(num_waitables_triggered != 0 && !x_event_arrived && !ui_queue_ready_to_read){
			continue;
		}

		render(*app);
	}

	wait_set.remove(ww.ui_queue);
	wait_set.remove(xew);

	return 0;
}

void application::set_fullscreen(bool enable){
#ifdef M_RASPBERRYPI
	if(this->is_fullscreen()){
		return;
	}
#endif
	if(enable == this->is_fullscreen()){
		return;
	}

	auto& ww = getImpl(this->windowPimpl);

	XEvent event;
	Atom stateAtom;
	Atom atom;

	stateAtom = XInternAtom(ww.display, "_NET_WM_STATE", False);
	atom = XInternAtom(ww.display, "_NET_WM_STATE_FULLSCREEN", False);

	event.xclient.type = ClientMessage;
	event.xclient.serial = 0;
	event.xclient.send_event = True;
	event.xclient.window = ww.window;
	event.xclient.message_type = stateAtom;
	event.xclient.format = 32;
	event.xclient.data.l[0]	= enable ? 1 : 0;
	event.xclient.data.l[1]	= atom;
	event.xclient.data.l[2]	= 0;

	XSendEvent(ww.display, DefaultRootWindow(ww.display), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);

	XFlush(ww.display);

	this->isFullscreen_v = enable;
}

void application::set_mouse_cursor_visible(bool visible){
	auto& ww = getImpl(this->windowPimpl);

	if(visible){
		XUndefineCursor(ww.display, ww.window);
	}else{
		XDefineCursor(ww.display, ww.window, ww.emptyCursor);
	}
}

void application::swapFrameBuffers(){
	auto& ww = getImpl(this->windowPimpl);

#ifdef MORDAVOKNE_RENDER_OPENGL2
	glXSwapBuffers(ww.display, ww.window);
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
	eglSwapBuffers(ww.eglDisplay, ww.eglSurface);
#else
#	error "Unknown graphics API"
#endif
}
