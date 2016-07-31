//This file contains implementations of platform dependent methods from App class.

#include <vector>
#include <array>

#include "../../AppFactory.hpp"
#include "../../../morda/util/util.hpp"

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
	std::vector<int> attr;
	attr.push_back(GLX_RGBA);
	attr.push_back(GLX_DOUBLEBUFFER);
	attr.push_back(GLX_RED_SIZE); attr.push_back(8);
	attr.push_back(GLX_GREEN_SIZE); attr.push_back(8);
	attr.push_back(GLX_BLUE_SIZE); attr.push_back(8);
	attr.push_back(GLX_ALPHA_SIZE); attr.push_back(8);
	
	if(wp.buffers.get(WindowParams::Buffer_e::DEPTH)){
		attr.push_back(GLX_DEPTH_SIZE); attr.push_back(24);
	}
	if(wp.buffers.get(WindowParams::Buffer_e::STENCIL)){
		attr.push_back(GLX_STENCIL_SIZE); attr.push_back(8);
	}
	
	attr.push_back(None);
	
	this->vi = glXChooseVisual(
			xDisplay.d,
			DefaultScreen(xDisplay.d),
			&*attr.begin()
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
	kolme::Vec2ui resolution(DisplayWidth(display, scrNum), DisplayHeight(display, scrNum));
	kolme::Vec2ui screenSizeMm(DisplayWidthMM(display, scrNum), DisplayHeightMM(display, scrNum));
	
	return findDotsPerPt(resolution, screenSizeMm);
}
}//~namespace

App::App(const WindowParams& requestedWindowParams) :
		xVisualInfo(requestedWindowParams, xDisplay),
		xWindow(requestedWindowParams, xDisplay, xVisualInfo),
		glxContex(xDisplay, xWindow, xVisualInfo),
		xEmptyMouseCursor(xDisplay, xWindow),
		xInputMethod(xDisplay, xWindow),
		gui(getDotsPerInch(xDisplay.d), ::getDotsPerPt(xDisplay.d))
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

Widget::MouseButton_e ButtonNumberToEnum(int number){
	switch(number){
		case 1:
			return Widget::MouseButton_e::LEFT;
		default:
		case 2:
			return Widget::MouseButton_e::MIDDLE;
		case 3:
			return Widget::MouseButton_e::RIGHT;
		case 4:
			return Widget::MouseButton_e::WHEEL_UP;
		case 5:
			return Widget::MouseButton_e::WHEEL_DOWN;
	}
}



const std::array<Key_e, std::uint8_t(-1) + 1> keyCodeMap = {{
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::ESCAPE,//9
	Key_e::ONE,//10
	Key_e::TWO,//11
	Key_e::THREE,//12
	Key_e::FOUR,//13
	Key_e::FIVE,//14
	Key_e::SIX,//15
	Key_e::SEVEN,//16
	Key_e::EIGHT,//17
	Key_e::NINE,//18
	Key_e::ZERO,//19
	Key_e::MINUS,//20
	Key_e::EQUALS,//21
	Key_e::BACKSPACE,//22
	Key_e::TAB,//23
	Key_e::Q,//24
	Key_e::W,//25
	Key_e::E,//26
	Key_e::R,//27
	Key_e::T,//28
	Key_e::Y,//29
	Key_e::U,//30
	Key_e::I,//31
	Key_e::O,//32
	Key_e::P,//33
	Key_e::LEFT_SQUARE_BRACKET,//34
	Key_e::RIGHT_SQUARE_BRACKET,//35
	Key_e::ENTER,//36
	Key_e::LEFT_CONTROL,//37
	Key_e::A,//38
	Key_e::S,//39
	Key_e::D,//40
	Key_e::F,//41
	Key_e::G,//42
	Key_e::H,//43
	Key_e::J,//44
	Key_e::K,//45
	Key_e::L,//46
	Key_e::SEMICOLON,//47
	Key_e::APOSTROPHE,//48
	Key_e::GRAVE,//49
	Key_e::LEFT_SHIFT,//50
	Key_e::BACKSLASH,//51
	Key_e::Z,//52
	Key_e::X,//53
	Key_e::C,//54
	Key_e::V,//55
	Key_e::B,//56
	Key_e::N,//57
	Key_e::M,//58
	Key_e::COMMA,//59
	Key_e::PERIOD,//60
	Key_e::SLASH,//61
	Key_e::RIGHT_SHIFT,//62
	Key_e::UNKNOWN,
	Key_e::LEFT_ALT,//64
	Key_e::SPACE,//65
	Key_e::CAPSLOCK,//66
	Key_e::F1,//67
	Key_e::F2,//68
	Key_e::F3,//69
	Key_e::F4,//70
	Key_e::F5,//71
	Key_e::F6,//72
	Key_e::F7,//73
	Key_e::F8,//74
	Key_e::F9,//75
	Key_e::F10,//76
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::F11,//95
	Key_e::F12,//96
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::RIGHT_CONTROL,//105
	Key_e::UNKNOWN,
	Key_e::PRINT_SCREEN,//107
	Key_e::RIGHT_ALT,//108
	Key_e::UNKNOWN,
	Key_e::HOME,//110
	Key_e::UP,//111
	Key_e::PAGE_UP,//112
	Key_e::LEFT,//113
	Key_e::RIGHT,//114
	Key_e::END,//115
	Key_e::DOWN,//116
	Key_e::PAGE_DOWN,//117
	Key_e::INSERT,//118
	Key_e::DELETE,//119
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::PAUSE,//127
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::WINDOWS,//133
	Key_e::UNKNOWN,
	Key_e::WINDOWS_MENU,//135
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN,
	Key_e::UNKNOWN
}};



class KeyEventUnicodeProvider : public morda::Morda::UnicodeProvider{
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
					return std::u32string();
				}
				return unikod::toUtf32(&*buf.begin());
			default:
			case XBufferOverflow:
				ASSERT(false)
			case XLookupKeySym:
			case XLookupNone:
				break;
		}//~switch

		return std::u32string();
	}
};



}//~namespace


void App::quit()noexcept{
	this->quitFlag = true;
}


void App::exec(){
	XEventWaitable xew(this->xDisplay.d);

	pogodi::WaitSet waitSet(2);

	waitSet.add(xew, pogodi::Waitable::READ);
	waitSet.add(this->uiQueue, pogodi::Waitable::READ);

	//Sometimes the first Expose event does not come for some reason. It happens constantly in some systems and never happens on all the others.
	//So, render everything for the first time.
	this->render();

	while(!this->quitFlag){
		waitSet.waitWithTimeout(this->gui.update());

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
							Key_e key = keyCodeMap[std::uint8_t(event.xkey.keycode)];
							this->handleKeyEvent(true, key);
							this->handleCharacterInput(KeyEventUnicodeProvider(this->xInputMethod.xic, event), key);
						}
						break;
					case KeyRelease:
//						TRACE(<< "KeyRelease X event got" << std::endl)
						{
							Key_e key = keyCodeMap[std::uint8_t(event.xkey.keycode)];

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
									this->handleCharacterInput(KeyEventUnicodeProvider(this->xInputMethod.xic, nev), key);

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
//						TRACE(<< "ButtonRelease X event got, button mask = " << event.xbutton.button << std::endl)
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
	std::unique_ptr<morda::App> app = createAppUnix(argc, argv, utki::Buf<std::uint8_t>());
	
	ASSERT(app)

	app->exec();
}

}



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

	this->isFullscreen_v = enable;
}



void App::setMouseCursorVisible(bool visible){
	if(visible){
		XUndefineCursor(this->xDisplay.d, this->xWindow.w);
	}else{
		XDefineCursor(this->xDisplay.d, this->xWindow.w, this->xEmptyMouseCursor.c);
	}
}



App::XEmptyMouseCursor::XEmptyMouseCursor(XDisplayWrapper& xDisplay, XWindowWrapper& xWindow) :
		d(xDisplay)
{
	Pixmap blank;
	XColor dummy;
	char data[1] = {0};

	blank = XCreateBitmapFromData(this->d.d, xWindow.w, data, 1, 1);
	if(blank == None){
		throw morda::Exc("App::XEmptyMouseCursor::XEmptyMouseCursor(): could not create bitmap");
	}
	this->c = XCreatePixmapCursor(this->d.d, blank, blank, &dummy, &dummy, 0, 0);
	XFreePixmap(this->d.d, blank);
}

App::XEmptyMouseCursor::~XEmptyMouseCursor()noexcept{
	XFreeCursor(this->d.d, this->c);
}
