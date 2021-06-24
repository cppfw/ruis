#include <vector>
#include <array>

#include "../../application.hpp"

#include <opros/wait_set.hpp>
#include <papki/fs_file.hpp>
#include <nitki/queue.hpp>

#include <utki/unicode.hpp>
#include <utki/string.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#ifdef MORDAVOKNE_RENDER_OPENGL2
#	include <GL/glew.h>
#	include <GL/glx.h>

#	include <morda/render/opengl2/renderer.hpp>

#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
#	include <EGL/egl.h>
#	ifdef MORDAVOKNE_RASPBERRYPI
#		include <bcm_host.h>
#	endif

#	include <morda/render/opengles2/renderer.hpp>

#else
#	error "Unknown graphics API"
#endif

#include "../friend_accessors.cxx"
#include "../unix_common.cxx"

using namespace mordavokne;

namespace{
const std::map<morda::mouse_cursor, unsigned> x_cursor_map = {
	{morda::mouse_cursor::arrow, XC_left_ptr},
	{morda::mouse_cursor::left_right_arrow, XC_sb_h_double_arrow},
	{morda::mouse_cursor::up_down_arrow, XC_sb_v_double_arrow},
	{morda::mouse_cursor::all_directions_arrow, XC_fleur},
	{morda::mouse_cursor::left_side, XC_left_side},
	{morda::mouse_cursor::right_side, XC_right_side},
	{morda::mouse_cursor::top_side, XC_top_side},
	{morda::mouse_cursor::bottom_side, XC_bottom_side},
	{morda::mouse_cursor::top_left_corner, XC_top_left_corner},
	{morda::mouse_cursor::top_right_corner, XC_top_right_corner},
	{morda::mouse_cursor::bottom_left_corner, XC_bottom_left_corner},
	{morda::mouse_cursor::bottom_right_corner, XC_bottom_right_corner},
	{morda::mouse_cursor::index_finger, XC_hand2},
	{morda::mouse_cursor::grab, XC_hand1},
	{morda::mouse_cursor::caret, XC_xterm}
};
}

namespace{
struct window_wrapper : public utki::destructable{
	struct display_wrapper{
		Display* display;

		display_wrapper(){
			this->display = XOpenDisplay(0);
			if(!this->display){
				throw std::runtime_error("XOpenDisplay() failed");
			}
		}

		~display_wrapper(){
			XCloseDisplay(this->display);
		}
	} display;

	Colormap color_map;
	::Window window;
#ifdef MORDAVOKNE_RENDER_OPENGL2
	GLXContext glContext;
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
#	ifdef MORDAVOKNE_RASPBERRYPI
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
	struct cursor_wrapper{
		window_wrapper& owner;
		Cursor cursor;
	
		cursor_wrapper(window_wrapper& owner, morda::mouse_cursor c) :
				owner(owner)
		{
			if(c == morda::mouse_cursor::none){
				Pixmap blank;
				XColor dummy;
				char data[1] = {0};

				blank = XCreateBitmapFromData(this->owner.display.display, this->owner.window, data, 1, 1);
				if(blank == None){
					throw std::runtime_error("application::XEmptyMouseCursor::XEmptyMouseCursor(): could not create bitmap");
				}
				utki::scope_exit scopeExit([this, &blank](){
					XFreePixmap(this->owner.display.display, blank);
				});
				this->cursor = XCreatePixmapCursor(this->owner.display.display, blank, blank, &dummy, &dummy, 0, 0);
			}else{
				this->cursor = XCreateFontCursor(this->owner.display.display, x_cursor_map.at(c));
			}
		}

		~cursor_wrapper(){
			XFreeCursor(this->owner.display.display, this->cursor);
		}
	};

	cursor_wrapper* cur_cursor = nullptr;
	bool cursor_visible = true;
	std::map<morda::mouse_cursor, std::unique_ptr<cursor_wrapper>> cursors;

	void apply_cursor(cursor_wrapper& c){
		XDefineCursor(
				this->display.display,
				this->window,
				c.cursor
			);
	}

	cursor_wrapper* get_cursor(morda::mouse_cursor c){
		auto i = this->cursors.find(c);
		if(i == this->cursors.end()){
			i = this->cursors.insert(std::make_pair(
					c,
					std::make_unique<cursor_wrapper>(*this, c)
				)).first;
		}
		return i->second.get();
	}

	void set_cursor(morda::mouse_cursor c){
		this->cur_cursor = this->get_cursor(c);

		if(this->cursor_visible){
			this->apply_cursor(*this->cur_cursor);
		}
	}

	void set_cursor_visible(bool visible){
		this->cursor_visible = visible;
		if(visible){
			if(this->cur_cursor){
				this->apply_cursor(*this->cur_cursor);
			}else{
				XUndefineCursor(this->display.display, this->window);
			}
		}else{
			this->apply_cursor(*this->get_cursor(morda::mouse_cursor::none));
		}
	}

	XIM inputMethod;
	XIC inputContext;

	nitki::queue ui_queue;

	volatile bool quitFlag = false;

	window_wrapper(const window_params& wp){
#ifdef MORDAVOKNE_RENDER_OPENGL2
		{
			int glxVerMajor, glxVerMinor;
			if(!glXQueryVersion(this->display.display, &glxVerMajor, &glxVerMinor)){
				throw std::runtime_error("glXQueryVersion() failed");
			}

			// we need the following:
			// - glXQueryExtensionsString(), availabale starting from GLX version 1.1
			// - FBConfigs, availabale starting from GLX version 1.3
			// minimum GLX version needed is 1.3
			if(glxVerMajor < 1 || (glxVerMajor == 1  && glxVerMinor < 3 )){
				throw std::runtime_error("GLX version 1.3 or above is required");
			}
		}

		GLXFBConfig best_fb_config;
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
			GLXFBConfig* fbc = glXChooseFBConfig(this->display.display, DefaultScreen(this->display.display), &*visualAttribs.begin(), &fbcount);
			if(!fbc){
				throw std::runtime_error("glXChooseFBConfig() returned empty list");
			}
			utki::scope_exit scopeExitFbc([&fbc](){
				XFree(fbc);
			});

			int best_fb_config_index = -1, worstFbc = -1, bestNumSamp = -1, worstNumSamp = 999;

			for(int i = 0; i < fbcount; ++i){
				XVisualInfo *vi = glXGetVisualFromFBConfig(this->display.display, fbc[i]);
				if(vi){
					int samp_buf, samples;
					glXGetFBConfigAttrib(this->display.display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
					glXGetFBConfigAttrib(this->display.display, fbc[i], GLX_SAMPLES, &samples);

					if ( best_fb_config_index < 0 || (samp_buf && samples > bestNumSamp )){
						best_fb_config_index = i, bestNumSamp = samples;
					}
					if ( worstFbc < 0 || !samp_buf || samples < worstNumSamp ){
						worstFbc = i, worstNumSamp = samples;
					}
				}
				XFree( vi );
			}
			best_fb_config = fbc[best_fb_config_index];
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

		XVisualInfo *visual_info;
#ifdef MORDAVOKNE_RENDER_OPENGL2
		visual_info = glXGetVisualFromFBConfig(this->display.display, best_fb_config);
		if(!visual_info){
			throw std::runtime_error("glXGetVisualFromFBConfig() failed");
		}
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
#	ifdef MORDAVOKNE_RASPBERRYPI
		{
			int numVisuals;
			XVisualInfo visTemplate;
			visTemplate.screen = DefaultScreen(this->display.display); // LCD
			visual_info = XGetVisualInfo(
					this->display.display,
					VisualScreenMask,
					&visTemplate,
					&numVisuals
				);
			if(!visual_info){
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
			visual_info = XGetVisualInfo(
					this->display.display,
					VisualIDMask,
					&visTemplate,
					&numVisuals
				);
			if(!visual_info){
				throw std::runtime_error("XGetVisualInfo() failed");
			}
		}
#	endif
#else
#	error "Unknown graphics API"
#endif
		utki::scope_exit scope_exit_visual_info([visual_info](){
			XFree(visual_info);
		});

		this->color_map = XCreateColormap(
				this->display.display,
				RootWindow(this->display.display, visual_info->screen),
				visual_info->visual,
				AllocNone
			);
		//TODO: check for error?
		utki::scope_exit scopeExitColorMap([this](){
			XFreeColormap(this->display.display, this->color_map);
		});

		{
			XSetWindowAttributes attr;
			attr.colormap = this->color_map;
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
					this->display.display,
					RootWindow(this->display.display, visual_info->screen),
					0,
					0,
					wp.dim.x(),
					wp.dim.y(),
					0,
					visual_info->depth,
					InputOutput,
					visual_info->visual,
					fields,
					&attr
				);
		}
		if(!this->window){
			throw std::runtime_error("Failed to create window");
		}
		utki::scope_exit scopeExitWindow([this](){
			XDestroyWindow(this->display.display, this->window);
		});

		{ // we want to handle WM_DELETE_WINDOW event to know when window is closed
			Atom a = XInternAtom(this->display.display, "WM_DELETE_WINDOW", True);
			XSetWMProtocols(this->display.display, this->window, &a, 1);
		}

		XMapWindow(this->display.display, this->window);

		XFlush(this->display.display);

#ifdef MORDAVOKNE_RENDER_OPENGL2
		// glXGetProcAddressARB() will retutn non-null pointer even if extension is not supported, so we
		// need to explicitly check for supported extensions.
		// SOURCE: https://dri.freedesktop.org/wiki/glXGetProcAddressNeverReturnsNULL/

		{
			auto glx_extensions = utki::split(std::string_view(glXQueryExtensionsString(this->display.display, visual_info->screen)));
			if(std::find(glx_extensions.begin(), glx_extensions.end(), "GLX_ARB_create_context") == glx_extensions.end()){
				// GLX_ARB_create_context is not supported
				this->glContext = glXCreateContext(this->display.display, visual_info, NULL, GL_TRUE);
			}else{
				// GLX_ARB_create_context is supported

				typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

				// NOTE: glXGetProcAddressARB() is guaranteed to be present in all GLX versions.
				//       glXGetProcAddress() is not guaranteed.
				// SOURCE: https://dri.freedesktop.org/wiki/glXGetProcAddressNeverReturnsNULL/

				glXCreateContextAttribsARBProc glXCreateContextAttribsARB = nullptr;
				glXCreateContextAttribsARB =
						(glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
				
				if(!glXCreateContextAttribsARB){
					// this should not happen since we checked above that we have GLX version >= 1.1
					throw std::runtime_error("glXCreateContextAttribsARB() not found");
				}

				// TODO: create latest possible OpenGL context?

				static int context_attribs[] = {
					GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
					GLX_CONTEXT_MINOR_VERSION_ARB, 0,
					None
				};

				this->glContext = glXCreateContextAttribsARB(this->display.display, best_fb_config, NULL, GL_TRUE, context_attribs);
			}

			// sync to ensure any errors generated are processed
			XSync(this->display.display, False);
		}
		
		if(this->glContext == NULL){
			throw std::runtime_error("glXCreateContext() failed");
		}
		utki::scope_exit scopeExitGLContext([this](){
			glXMakeCurrent(this->display.display, None, NULL);
			glXDestroyContext(this->display.display, this->glContext);
		});

		glXMakeCurrent(this->display.display, this->window, this->glContext);

		TRACE(<< "OpenGL version: " << glGetString(GL_VERSION) << std::endl)

		if(glewInit() != GLEW_OK){
			throw std::runtime_error("GLEW initialization failed");
		}
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)

#	ifdef MORDAVOKNE_RASPBERRYPI
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

			this->rpiDispmanDisplay = vc_dispmanx_display_open(0); // 0 = LCD
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
#	ifdef MORDAVOKNE_RASPBERRYPI
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
		this->inputMethod = XOpenIM(this->display.display, NULL, NULL, NULL);
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
		scopeExitWindow.reset();
		scopeExitColorMap.reset();
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
	~window_wrapper()noexcept{
		XUnsetICFocus(this->inputContext);
		XDestroyIC(this->inputContext);

		XCloseIM(this->inputMethod);

#ifdef MORDAVOKNE_RENDER_OPENGL2
		glXMakeCurrent(this->display.display, None, NULL);
		glXDestroyContext(this->display.display, this->glContext);
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
		eglMakeCurrent(this->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(this->eglDisplay, this->eglContext);
		eglDestroySurface(this->eglDisplay, this->eglSurface);
#else
#	error "Unknown graphics API"
#endif

		XDestroyWindow(this->display.display, this->window);
		XFreeColormap(this->display.display, this->color_map);

#ifdef MORDAVOKNE_RENDER_OPENGLES2
		eglTerminate(this->eglDisplay);
#endif
	}
};

window_wrapper& getImpl(const std::unique_ptr<utki::destructable>& pimpl){
	ASSERT(dynamic_cast<window_wrapper*>(pimpl.get()))
	return static_cast<window_wrapper&>(*pimpl);
}

window_wrapper& get_impl(application& app){
	return getImpl(get_window_pimpl(app));
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
		window_pimpl(std::make_unique<window_wrapper>(requestedWindowParams)),
		gui(std::make_shared<morda::context>(
#ifdef MORDAVOKNE_RENDER_OPENGL2
				std::make_shared<morda::render_opengl2::renderer>(),
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
				std::make_shared<morda::render_opengles2::renderer>(),
#else
#	error "Unknown graphics API"
#endif
				std::make_shared<morda::updater>(),
				[this](std::function<void()>&& a){
					getImpl(get_window_pimpl(*this)).ui_queue.push_back(std::move(a));
				},
				[this](morda::mouse_cursor c){
					auto& ww = get_impl(*this);
					ww.set_cursor(c);
				},
				getDotsPerInch(getImpl(window_pimpl).display.display),
				::getDotsPerPt(getImpl(window_pimpl).display.display)
			)),
		storage_dir(initialize_storage_dir(this->name))
{
#ifdef MORDAVOKNE_RASPBERRYPI
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
	auto& ww = getImpl(this->window_pimpl);

	ww.quitFlag = true;
}

int main(int argc, const char** argv){
	std::unique_ptr<mordavokne::application> app = createAppUnix(argc, argv);
	if(!app){
		return 0;
	}

	ASSERT(app)

	auto& ww = getImpl(get_window_pimpl(*app));

	XEvent_waitable xew(ww.display.display);

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

		morda::vector2 new_win_dims(-1, -1);

		// NOTE: do not check 'read' flag for X event, for some reason when waiting with 0 timeout it will never be set.
		//       Maybe some bug in XWindows, maybe something else.
		bool x_event_arrived = false;
		while(XPending(ww.display.display) > 0){
			x_event_arrived = true;
			XEvent event;
			XNextEvent(ww.display.display, &event);
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
					// squash all window resize events into one, for that store the new window dimensions and update the
					// viewport later only once
					new_win_dims.x() = morda::real(event.xconfigure.width);
					new_win_dims.y() = morda::real(event.xconfigure.height);
					break;
				case KeyPress:
//						TRACE(<< "KeyPress X event got" << std::endl)
					{
						morda::key key = keyCodeMap[std::uint8_t(event.xkey.keycode)];
						handle_key_event(*app, true, key);
						handle_character_input(*app, KeyEventUnicodeProvider(ww.inputContext, event), key);
					}
					break;
				case KeyRelease:
//						TRACE(<< "KeyRelease X event got" << std::endl)
					{
						morda::key key = keyCodeMap[std::uint8_t(event.xkey.keycode)];

						// detect auto-repeated key events
						if(XEventsQueued(ww.display.display, QueuedAfterReading)){ // if there are other events queued
							XEvent nev;
							XPeekEvent(ww.display.display, &nev);

							if(nev.type == KeyPress
									&& nev.xkey.time == event.xkey.time
									&& nev.xkey.keycode == event.xkey.keycode
								)
							{
								// key wasn't actually released
								handle_character_input(*app, KeyEventUnicodeProvider(ww.inputContext, nev), key);

								XNextEvent(ww.display.display, &nev); // remove the key down event from queue
								break;
							}
						}

						handle_key_event(*app, false, key);
					}
					break;
				case ButtonPress:
//						TRACE(<< "ButtonPress X event got, button mask = " << event.xbutton.button << std::endl)
//						TRACE(<< "ButtonPress X event got, x, y = " << event.xbutton.x << ", " << event.xbutton.y << std::endl)
					handle_mouse_button(
							*app,
							true,
							morda::vector2(event.xbutton.x, event.xbutton.y),
							buttonNumberToEnum(event.xbutton.button),
							0
						);
					break;
				case ButtonRelease:
//						TRACE(<< "ButtonRelease X event got, button mask = " << event.xbutton.button << std::endl)
					handle_mouse_button(
							*app,
							false,
							morda::vector2(event.xbutton.x, event.xbutton.y),
							buttonNumberToEnum(event.xbutton.button),
							0
						);
					break;
				case MotionNotify:
//						TRACE(<< "MotionNotify X event got" << std::endl)
					handle_mouse_move(
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
						char* name = XGetAtomName(ww.display.display, event.xclient.message_type);
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

		if(new_win_dims.is_positive_or_zero()){
			update_window_rect(*app, morda::rectangle(0, new_win_dims));
		}

		render(*app);
	}

	wait_set.remove(ww.ui_queue);
	wait_set.remove(xew);

	return 0;
}

void application::set_fullscreen(bool enable){
#ifdef MORDAVOKNE_RASPBERRYPI
	if(this->is_fullscreen()){
		return;
	}
#endif
	if(enable == this->is_fullscreen()){
		return;
	}

	auto& ww = getImpl(this->window_pimpl);

	XEvent event;
	Atom stateAtom;
	Atom atom;

	stateAtom = XInternAtom(ww.display.display, "_NET_WM_STATE", False);
	atom = XInternAtom(ww.display.display, "_NET_WM_STATE_FULLSCREEN", False);

	event.xclient.type = ClientMessage;
	event.xclient.serial = 0;
	event.xclient.send_event = True;
	event.xclient.window = ww.window;
	event.xclient.message_type = stateAtom;
	event.xclient.format = 32;
	event.xclient.data.l[0]	= enable ? 1 : 0;
	event.xclient.data.l[1]	= atom;
	event.xclient.data.l[2]	= 0;

	XSendEvent(
			ww.display.display,
			DefaultRootWindow(ww.display.display),
			False,
			SubstructureRedirectMask | SubstructureNotifyMask,
			&event
		);

	XFlush(ww.display.display);

	this->isFullscreen_v = enable;
}

void application::set_mouse_cursor_visible(bool visible){
	get_impl(*this).set_cursor_visible(visible);
}

void application::swap_frame_buffers(){
	auto& ww = getImpl(this->window_pimpl);

#ifdef MORDAVOKNE_RENDER_OPENGL2
	glXSwapBuffers(ww.display.display, ww.window);
#elif defined(MORDAVOKNE_RENDER_OPENGLES2)
	eglSwapBuffers(ww.eglDisplay, ww.eglSurface);
#else
#	error "Unknown graphics API"
#endif
}
