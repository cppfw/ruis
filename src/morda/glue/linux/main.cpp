//This file contains implementations of platform dependent methods from App class.

#include <vector>
#include <array>

#include "../../AppFactory.hpp"

#include <pogodi/WaitSet.hpp>
#include <papki/FSFile.hpp>
#include <unikod/utf8.hpp>




using namespace morda;



App::XDisplayWrapper::XDisplayWrapper(){
	this->d = XOpenDisplay(0);
	if(!this->d){
		throw morda::Exc("XOpenDisplay() failed");
	}
}



App::XDisplayWrapper::~XDisplayWrapper()noexcept{
	XCloseDisplay(this->d);
}



App::XVisualInfoWrapper::XVisualInfoWrapper(const WindowParams& wp, XDisplayWrapper& xDisplay){
	//TODO: allow configuring depth, stencil buffers via WindowParams
	int attr[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		None
	};

	this->vi = glXChooseVisual(
			xDisplay.d,
			DefaultScreen(xDisplay.d),
			attr
		);
	if(!this->vi){
		throw morda::Exc("glXChooseVisual() failed");
	}
}



App::XVisualInfoWrapper::~XVisualInfoWrapper()noexcept{
	XFree(this->vi);
}



App::XWindowWrapper::XWindowWrapper(const App::WindowParams& wp, XDisplayWrapper& xDisplay, XVisualInfoWrapper& xVisualInfo) :
		d(xDisplay)
{
	Colormap colorMap = XCreateColormap(
			this->d.d,
			RootWindow(this->d.d, xVisualInfo.vi->screen),
			xVisualInfo.vi->visual,
			AllocNone
		);
	//TODO: check for error

	XSetWindowAttributes attr;
	attr.colormap = colorMap;
	attr.border_pixel = 0;
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

	this->w = XCreateWindow(
			this->d.d,
			RootWindow(this->d.d, xVisualInfo.vi->screen),
			0,
			0,
			wp.dim.x,
			wp.dim.y,
			0,
			xVisualInfo.vi->depth,
			InputOutput,
			xVisualInfo.vi->visual,
			fields,
			&attr
		);
	//TODO: check for error

	{//We want to handle WM_DELETE_WINDOW event to know when window is closed.
		Atom a = XInternAtom(this->d.d, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(this->d.d, this->w, &a, 1);
	}

	XMapWindow(this->d.d, this->w);

	XFlush(this->d.d);
}



App::XWindowWrapper::~XWindowWrapper()noexcept{
	XDestroyWindow(this->d.d, this->w);
}



App::GLXContextWrapper::GLXContextWrapper(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow, XVisualInfoWrapper& xVisualInfo) :
		d(xDisplay),
		w(xWindow)
{
	this->glxContext = glXCreateContext(this->d.d, xVisualInfo.vi, 0, GL_TRUE);
	if(this->glxContext == NULL){
		throw morda::Exc("glXCreateContext() failed");
	}
	glXMakeCurrent(this->d.d, this->w.w, this->glxContext);

	TRACE(<< "OpenGL version: " << glGetString(GL_VERSION) << std::endl)
}



void App::GLXContextWrapper::Destroy()noexcept{
//	TRACE(<< "App::GLXContextWrapper::Destroy(): invoked" << std::endl)
	glXMakeCurrent(this->d.d, None, NULL);
//	TRACE(<< "App::GLXContextWrapper::Destroy(): destroying context" << std::endl)
	glXDestroyContext(this->d.d, this->glxContext);
}



App::XInputMethodWrapper::XInputMethodWrapper(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow) :
		d(xDisplay),
		w(xWindow)
{
	this->xim = XOpenIM(this->d.d, NULL, NULL, NULL);
	if(this->xim == NULL){
		throw morda::Exc("XOpenIM() failed");
	}

	this->xic = XCreateIC(
			this->xim,
			XNClientWindow, this->w.w,
			XNFocusWindow, this->w.w,
			XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
			NULL
		);
	if(this->xic == NULL){
		this->Destroy();
	}
}



void App::XInputMethodWrapper::Destroy()noexcept{
	if(this->xic != NULL){
		XUnsetICFocus(this->xic);
		XDestroyIC(this->xic);
	}
	if(this->xim != NULL){
		XCloseIM(this->xim);
	}
}


namespace{
real getDotsPerInch(Display* display){
	int scrNum = 0;
	real value = ((real(DisplayWidth(display, scrNum)) / (real(DisplayWidthMM(display, scrNum))/ 10.0))
			+ (real(DisplayHeight(display, scrNum)) / (real(DisplayHeightMM(display, scrNum)) / 10.0))) / 2;
	value *= 2.54f;
	return value;
}

real getDotsPerPt(Display* display){
	int scrNum = 0;
	return real(std::min(DisplayWidth(display, scrNum), DisplayHeight(display, scrNum))) / morda::screenSizePt;
}
}//~namespace

App::App(const WindowParams& requestedWindowParams) :
		xVisualInfo(requestedWindowParams, xDisplay),
		xWindow(requestedWindowParams, xDisplay, xVisualInfo),
		glxContex(xDisplay, xWindow, xVisualInfo),
		xInputMethod(xDisplay, xWindow),
		units(getDotsPerInch(xDisplay.d), getDotsPerPt(xDisplay.d))
{
#ifdef DEBUG
	//print GLX version
	{
		int major, minor;
		glXQueryVersion(this->xDisplay.d, &major, &minor);
		TRACE(<< "GLX Version: " << major << "." << minor << std::endl)
	}
#endif
}



namespace{

class XEventWaitable : public pogodi::Waitable{
	int fd;

	int getHandle() override{
		return this->fd;
	}
public:
	XEventWaitable(Display* d){
		this->fd = XConnectionNumber(d);
	}

	void clearCanReadFlag(){
		this->pogodi::Waitable::clearCanReadFlag();
	}
};

Widget::EMouseButton ButtonNumberToEnum(int number){
	switch(number){
		case 1:
			return Widget::EMouseButton::LEFT;
		default:
		case 2:
			return Widget::EMouseButton::MIDDLE;
		case 3:
			return Widget::EMouseButton::RIGHT;
		case 4:
			return Widget::EMouseButton::WHEEL_UP;
		case 5:
			return Widget::EMouseButton::WHEEL_DOWN;
	}
}



const std::array<EKey, std::uint8_t(-1) + 1> keyCodeMap = {{
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::ESCAPE,//9
	EKey::ONE,//10
	EKey::TWO,//11
	EKey::THREE,//12
	EKey::FOUR,//13
	EKey::FIVE,//14
	EKey::SIX,//15
	EKey::SEVEN,//16
	EKey::EIGHT,//17
	EKey::NINE,//18
	EKey::ZERO,//19
	EKey::MINUS,//20
	EKey::EQUALS,//21
	EKey::BACKSPACE,//22
	EKey::TAB,//23
	EKey::Q,//24
	EKey::W,//25
	EKey::E,//26
	EKey::R,//27
	EKey::T,//28
	EKey::Y,//29
	EKey::U,//30
	EKey::I,//31
	EKey::O,//32
	EKey::P,//33
	EKey::LEFT_SQUARE_BRACKET,//34
	EKey::RIGHT_SQUARE_BRACKET,//35
	EKey::ENTER,//36
	EKey::LEFT_CONTROL,//37
	EKey::A,//38
	EKey::S,//39
	EKey::D,//40
	EKey::F,//41
	EKey::G,//42
	EKey::H,//43
	EKey::J,//44
	EKey::K,//45
	EKey::L,//46
	EKey::SEMICOLON,//47
	EKey::APOSTROPHE,//48
	EKey::GRAVE,//49
	EKey::LEFT_SHIFT,//50
	EKey::BACKSLASH,//51
	EKey::Z,//52
	EKey::X,//53
	EKey::C,//54
	EKey::V,//55
	EKey::B,//56
	EKey::N,//57
	EKey::M,//58
	EKey::COMMA,//59
	EKey::PERIOD,//60
	EKey::SLASH,//61
	EKey::RIGHT_SHIFT,//62
	EKey::UNKNOWN,
	EKey::LEFT_ALT,//64
	EKey::SPACE,//65
	EKey::CAPSLOCK,//66
	EKey::F1,//67
	EKey::F2,//68
	EKey::F3,//69
	EKey::F4,//70
	EKey::F5,//71
	EKey::F6,//72
	EKey::F7,//73
	EKey::F8,//74
	EKey::F9,//75
	EKey::F10,//76
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::F11,//95
	EKey::F12,//96
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::RIGHT_CONTROL,//105
	EKey::UNKNOWN,
	EKey::PRINT_SCREEN,//107
	EKey::RIGHT_ALT,//108
	EKey::UNKNOWN,
	EKey::HOME,//110
	EKey::UP,//111
	EKey::PAGE_UP,//112
	EKey::LEFT,//113
	EKey::RIGHT,//114
	EKey::END,//115
	EKey::DOWN,//116
	EKey::PAGE_DOWN,//117
	EKey::INSERT,//118
	EKey::DELETE,//119
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::PAUSE,//127
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::WINDOWS,//133
	EKey::UNKNOWN,
	EKey::WINDOWS_MENU,//135
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN,
	EKey::UNKNOWN
}};



class KeyEventUnicodeResolver{
	XIC& xic;
	XEvent& event;
public:
	KeyEventUnicodeResolver(XIC& xic, XEvent& event) :
			xic(xic),
			event(event)
	{}

	std::vector<std::uint32_t> Resolve()const{
#ifndef X_HAVE_UTF8_STRING
#	error "no Xutf8stringlookup()"
#endif

		Status status;
		//KeySym xkeysym;
		std::array<char, 32> staticBuf;
		std::vector<char> arr;
		auto buf = utki::wrapBuf(staticBuf);

		int size = Xutf8LookupString(this->xic, &this->event.xkey, buf.begin(), buf.size() - 1, NULL, &status);
		if(status == XBufferOverflow){
			//allocate enough memory
			arr.resize(size + 1);
			buf = utki::wrapBuf(arr);
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
					return std::vector<std::uint32_t>();
				}

				{
					typedef std::vector<std::uint32_t> T_Vector;
					T_Vector utf32;

					for(unikod::Utf8Iterator i(buf.begin()); !i.isEnd(); ++i){
						utf32.push_back(i.character());
					}

					std::vector<std::uint32_t> ret(utf32.size());

					std::uint32_t* dst = &*ret.begin();
					for(T_Vector::iterator src = utf32.begin(); src != utf32.end(); ++src, ++dst){
						*dst = *src;
					}

					return ret;
				}

				break;
			default:
			case XBufferOverflow:
				ASSERT(false)
			case XLookupKeySym:
			case XLookupNone:
				break;
		}//~switch

		return std::vector<std::uint32_t>();
	}
};



}//~namespace


void App::quit()noexcept{
	this->quitFlag = true;
}


void App::Exec(){
	XEventWaitable xew(this->xDisplay.d);

	pogodi::WaitSet waitSet(2);

	waitSet.add(xew, pogodi::Waitable::READ);
	waitSet.add(this->uiQueue, pogodi::Waitable::READ);

	//Sometimes the first Expose event does not come for some reason. It happens constantly in some systems and never happens on all the others.
	//So, render everything for the first time.
	this->render();

	while(!this->quitFlag){
		waitSet.waitWithTimeout(this->updater.update());

		if(this->uiQueue.canRead()){
			while(auto m = this->uiQueue.peekMsg()){
				m();
			}
			ASSERT(!this->uiQueue.canRead())
		}

		if(xew.canRead()){
			xew.clearCanReadFlag();
			while(XPending(this->xDisplay.d) > 0){
				XEvent event;
				XNextEvent(this->xDisplay.d, &event);
//				TRACE(<< "X event got, type = " << (event.type) << std::endl)
				switch(event.type){
					case Expose:
//						TRACE(<< "Expose X event got" << std::endl)
						if(event.xexpose.count != 0){
							break;//~switch()
						}
						this->render();
						break;
					case ConfigureNotify:
//						TRACE(<< "ConfigureNotify X event got" << std::endl)
						this->updateWindowRect(morda::Rectr(0, 0, float(event.xconfigure.width), float(event.xconfigure.height)));
						break;
					case KeyPress:
//						TRACE(<< "KeyPress X event got" << std::endl)
						{
							EKey key = keyCodeMap[std::uint8_t(event.xkey.keycode)];
							this->handleKeyEvent(true, key);
							this->handleCharacterInput(KeyEventUnicodeResolver(this->xInputMethod.xic, event), key);
						}
						break;
					case KeyRelease:
//						TRACE(<< "KeyRelease X event got" << std::endl)
						{
							EKey key = keyCodeMap[std::uint8_t(event.xkey.keycode)];

							//detect auto-repeated key events
							if(XEventsQueued(this->xDisplay.d, QueuedAfterReading)){//if there are other events queued
								XEvent nev;
								XPeekEvent(this->xDisplay.d, &nev);

								if(nev.type == KeyPress
										&& nev.xkey.time == event.xkey.time
										&& nev.xkey.keycode == event.xkey.keycode
									)
								{
									//Key wasn't actually released
									this->handleCharacterInput(KeyEventUnicodeResolver(this->xInputMethod.xic, nev), key);

									XNextEvent(this->xDisplay.d, &nev);//remove the key down event from queue
									break;
								}
							}

							this->handleKeyEvent(false, key);
						}
						break;
					case ButtonPress:
//						TRACE(<< "ButtonPress X event got, button mask = " << event.xbutton.button << std::endl)
//						TRACE(<< "ButtonPress X event got, x, y = " << event.xbutton.x << ", " << event.xbutton.y << std::endl)
						this->handleMouseButton(
								true,
								morda::Vec2r(event.xbutton.x, event.xbutton.y),
								ButtonNumberToEnum(event.xbutton.button),
								0
							);
						break;
					case ButtonRelease:
						this->handleMouseButton(
								false,
								morda::Vec2r(event.xbutton.x, event.xbutton.y),
								ButtonNumberToEnum(event.xbutton.button),
								0
							);
						break;
					case MotionNotify:
//						TRACE(<< "MotionNotify X event got" << std::endl)
						this->handleMouseMove(
								morda::Vec2r(event.xmotion.x, event.xmotion.y),
								0
							);
						break;
					case EnterNotify:
						this->handleMouseHover(true, 0);
						break;
					case LeaveNotify:
						this->handleMouseHover(false, 0);
						break;
					case ClientMessage:
//						TRACE(<< "ClientMessage X event got" << std::endl)
						//probably a WM_DELETE_WINDOW event
						{
							char* name = XGetAtomName(this->xDisplay.d, event.xclient.message_type);
							if(*name == *"WM_PROTOCOLS"){
								this->quitFlag = true;
							}
							XFree(name);
						}
						break;
					default:
						//ignore
						break;
				}//~switch()
			}//~while()
		}//~if there are pending X events

		this->render();
	}//~while(!this->quitFlag)

	waitSet.remove(this->uiQueue);
	waitSet.remove(xew);
}



namespace morda{

inline void Main(int argc, const char** argv){
	std::unique_ptr<morda::App> app = morda::createApp(argc, argv, utki::Buf<std::uint8_t>(0, 0));

	app->Exec();
}

}//~namespace



int main(int argc, const char** argv){
	morda::Main(argc, argv);

	return 0;
}



void App::setFullscreen(bool enable){
	if(enable == this->isFullscreen()){
		return;
	}

	XEvent event;
	Atom stateAtom;
	Atom atom;

	stateAtom = XInternAtom(this->xDisplay.d, "_NET_WM_STATE", False);
	atom = XInternAtom(this->xDisplay.d, "_NET_WM_STATE_FULLSCREEN", False);

	event.xclient.type = ClientMessage;
	event.xclient.serial = 0;
	event.xclient.send_event = True;
	event.xclient.window = this->xWindow.w;
	event.xclient.message_type = stateAtom;
	event.xclient.format = 32;
	event.xclient.data.l[0]	= enable ? 1 : 0;
	event.xclient.data.l[1]	= atom;
	event.xclient.data.l[2]	= 0;

	XSendEvent(this->xDisplay.d, DefaultRootWindow(this->xDisplay.d), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);

	XFlush(this->xDisplay.d);

	this->isFullscreen_var = enable;
}
