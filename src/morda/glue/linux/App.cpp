//This file contains implementations of platform dependent methods from App class.

#include <vector>

#include "../../App.hpp"

#include <ting/WaitSet.hpp>
#include <ting/fs/FSFile.hpp>
#include <ting/utf8.hpp>
#include <ting/util.hpp>



using namespace morda;



App::XDisplayWrapper::XDisplayWrapper(){
	this->d = XOpenDisplay(0);
	if(!this->d){
		throw morda::Exc("XOpenDisplay() failed");
	}
}



App::XDisplayWrapper::~XDisplayWrapper()throw(){
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



App::XVisualInfoWrapper::~XVisualInfoWrapper()throw(){
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
			CWBorderPixel | CWColormap | CWEventMask,
			&attr
		);
	//TODO: check for error
	
	{//We want to handle WM_DELETE_WINDOW event to know when window is closed.
		Atom a = XInternAtom(this->d.d, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(this->d.d, this->w, &a, 1);
	}

	XMapWindow(this->d.d, this->w);
}



App::XWindowWrapper::~XWindowWrapper()throw(){
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
	
	if(glewInit() != GLEW_OK){
		this->Destroy();
		throw morda::Exc("GLEW initialization failed");
	}
}



void App::GLXContextWrapper::Destroy()throw(){
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



void App::XInputMethodWrapper::Destroy()throw(){
	if(this->xic != NULL){
		XUnsetICFocus(this->xic);
		XDestroyIC(this->xic);
	}
	if(this->xim != NULL){
		XCloseIM(this->xim);
	}
}


App::App(const WindowParams& requestedWindowParams) :
		xVisualInfo(requestedWindowParams, xDisplay),
		xWindow(requestedWindowParams, xDisplay, xVisualInfo),
		glxContex(xDisplay, xWindow, xVisualInfo),
		xInputMethod(xDisplay, xWindow),
		curWinRect(0, 0, -1, -1)
{
//	{
//		int scrNum = 0;
//		this->dotsPerMm = ((double(DisplayWidth(this->xDisplay.d, scrNum)) / double(DisplayWidthMM(this->xDisplay.d, scrNum)))
//				+ (double(DisplayHeight(this->xDisplay.d, scrNum)) / double(DisplayHeightMM(this->xDisplay.d, scrNum)))) / 2;
//	}
	this->dotsPerMm = 72.0f / 25.4f;
	
#ifdef DEBUG
	//print GLX version
	{
		int major, minor;
		glXQueryVersion(this->xDisplay.d, &major, &minor);
		TRACE(<< "GLX Version: " << major << "." << minor << std::endl)
	}
#endif
	
	this->UpdateWindowRect(
			morda::Rect2f(
					0,
					0,
					float(requestedWindowParams.dim.x),
					float(requestedWindowParams.dim.y)
				)
		);
}



ting::Ptr<ting::fs::File> App::CreateResourceFileInterface(const std::string& path)const{
	ting::Ptr<ting::fs::FSFile> fi = ting::fs::FSFile::New(path);
	fi->SetRootDir("res/");
	return fi;
}



namespace{

class XEventWaitable : public ting::Waitable{
	int fd;
	
	int GetHandle() OVERRIDE{
		return this->fd;
	}
public:
	XEventWaitable(Display* d){
		this->fd = XConnectionNumber(d);
	}
	
	inline void ClearCanReadFlag(){
		this->ting::Waitable::ClearCanReadFlag();
	}
};

Widget::EMouseButton ButtonNumberToEnum(int number){
	switch(number){
		case 1:
			return Widget::LEFT;
		default:
		case 2:
			return Widget::MIDDLE;
		case 3:
			return Widget::RIGHT;
		case 4:
			return Widget::WHEEL_UP;
		case 5:
			return Widget::WHEEL_DOWN;
	}
}



const key::Key keyCodeMap[ting::u8(-1) + 1] = {
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::ESCAPE,//9
	key::ONE,//10
	key::TWO,//11
	key::THREE,//12
	key::FOUR,//13
	key::FIVE,//14
	key::SIX,//15
	key::SEVEN,//16
	key::EIGHT,//17
	key::NINE,//18
	key::ZERO,//19
	key::MINUS,//20
	key::EQUALS,//21
	key::BACKSPACE,//22
	key::TAB,//23
	key::Q,//24
	key::W,//25
	key::E,//26
	key::R,//27
	key::T,//28
	key::Y,//29
	key::U,//30
	key::I,//31
	key::O,//32
	key::P,//33
	key::LEFT_SQUARE_BRACKET,//34
	key::RIGHT_SQUARE_BRACKET,//35
	key::ENTER,//36
	key::LEFT_CONTROL,//37
	key::A,//38
	key::S,//39
	key::D,//40
	key::F,//41
	key::G,//42
	key::H,//43
	key::J,//44
	key::K,//45
	key::L,//46
	key::SEMICOLON,//47
	key::APOSTROPHE,//48
	key::TICK,//49
	key::LEFT_SHIFT,//50
	key::BACKSLASH,//51
	key::Z,//52
	key::X,//53
	key::C,//54
	key::V,//55
	key::B,//56
	key::N,//57
	key::M,//58
	key::COMMA,//59
	key::PERIOD,//60
	key::SLASH,//61
	key::RIGHT_SHIFT,//62
	key::UNKNOWN,
	key::LEFT_ALT,//64
	key::SPACE,//65
	key::CAPSLOCK,//66
	key::F1,//67
	key::F2,//68
	key::F3,//69
	key::F4,//70
	key::F5,//71
	key::F6,//72
	key::F7,//73
	key::F8,//74
	key::F9,//75
	key::F10,//76
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::F11,//95
	key::F12,//96
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::RIGHT_CONTROL,//105
	key::UNKNOWN,
	key::PRINT_SCREEN,//107
	key::RIGHT_ALT,//108
	key::UNKNOWN,
	key::HOME,//110
	key::UP,//111
	key::PAGE_UP,//112
	key::LEFT,//113
	key::RIGHT,//114
	key::END,//115
	key::DOWN,//116
	key::PAGE_DOWN,//117
	key::INSERT,//118
	key::DELETE,//119
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::PAUSE,//127
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::WINDOWS,//133
	key::UNKNOWN,
	key::WINDOWS_MENU,//135
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN,
	key::UNKNOWN
};



class KeyEventUnicodeResolver{
	XIC& xic;
	XEvent& event;
public:
	KeyEventUnicodeResolver(XIC& xic, XEvent& event) :
			xic(xic),
			event(event)
	{}
	
	ting::Array<ting::u32> Resolve()const{
#ifndef X_HAVE_UTF8_STRING
#	error "no Xutf8stringlookup()"
#endif
		
		Status status;
		//KeySym xkeysym;
		ting::StaticBuffer<char, 32> staticBuf;
		ting::Array<char> arr;
		ting::Buffer<char>* buf = &staticBuf;

		int size = Xutf8LookupString(this->xic, &this->event.xkey, buf->Begin(), buf->Size() - 1, NULL, &status);
		if(status == XBufferOverflow){
			//allocate enough memory
			arr.Init(size + 1);
			buf = &arr;
			size = Xutf8LookupString(this->xic, &this->event.xkey, buf->Begin(), buf->Size() - 1, NULL, &status);
		}
		ASSERT(size >= 0)
		ASSERT(buf->Size() != 0)
		ASSERT(buf->Size() > unsigned(size))
		
		TRACE(<< "KeyEventUnicodeResolver::Resolve(): size = " << size << std::endl)
		
		(*buf)[size] = 0;//null-terminate
		
		switch(status){
			case XLookupChars:
			case XLookupBoth:
				if(size == 0){
					return ting::Array<ting::u32>();
				}
				
				{
					typedef std::vector<ting::u32> T_Vector;
					T_Vector utf32;
					
					for(ting::utf8::Iterator i(buf->Begin()); i.IsNotEnd(); ++i){
						utf32.push_back(i.Char());
					}
					
					ting::Array<ting::u32> ret(utf32.size());
					
					ting::u32* dst = ret.Begin();
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
		
		return ting::Array<ting::u32>();
	}
};



}//~namespace



void App::Exec(){
	
	XEventWaitable xew(this->xDisplay.d);
	
	ting::WaitSet waitSet(2);
	
	waitSet.Add(xew, ting::Waitable::READ);
//	waitSet.Add(this->queue, ting::Waitable::READ);
	
	//Sometimes the first Expose event does not come for some reason. It happens constantly in some systems and never happens on all the others.
	//So, render everything for the first time.
	this->Render();
	
	bool quitFlag = false;
	
	while(!quitFlag){
		waitSet.WaitWithTimeout(this->updater.Update());
		
		if(xew.CanRead()){
			xew.ClearCanReadFlag();
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
						this->Render();
						break;
					case ConfigureNotify:
//						TRACE(<< "ConfigureNotify X event got" << std::endl)
						this->UpdateWindowRect(morda::Rect2f(0, 0, float(event.xconfigure.width), float(event.xconfigure.height)));
						break;
					case KeyPress:
//						TRACE(<< "KeyPress X event got" << std::endl)
						this->HandleKeyEvent(true, keyCodeMap[ting::u8(event.xkey.keycode)]);
						this->HandleCharacterInput(KeyEventUnicodeResolver(this->xInputMethod.xic, event));
						break;
					case KeyRelease:
//						TRACE(<< "KeyRelease X event got" << std::endl)
						
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
								this->HandleCharacterInput(KeyEventUnicodeResolver(this->xInputMethod.xic, nev));
								
								XNextEvent(this->xDisplay.d, &nev);//remove the key down event from queue
								break;
							}
						}

						this->HandleKeyEvent(false, keyCodeMap[ting::u8(event.xkey.keycode)]);
						break;
					case ButtonPress:
//						TRACE(<< "ButtonPress X event got, button mask = " << event.xbutton.button << std::endl)
//						TRACE(<< "ButtonPress X event got, x, y = " << event.xbutton.x << ", " << event.xbutton.y << std::endl)
						this->HandleMouseButton(
								true,
								morda::Vec2f(event.xbutton.x, event.xbutton.y),
								ButtonNumberToEnum(event.xbutton.button),
								0
							);
						break;
					case ButtonRelease:
						this->HandleMouseButton(
								false,
								morda::Vec2f(event.xbutton.x, event.xbutton.y),
								ButtonNumberToEnum(event.xbutton.button),
								0
							);
						break;
					case MotionNotify:
//						TRACE(<< "MotionNotify X event got" << std::endl)
						this->HandleMouseMove(
								morda::Vec2f(event.xmotion.x, event.xmotion.y),
								0
							);
						break;
					case EnterNotify:
						this->HandleMouseHover(true);
						break;
					case LeaveNotify:
						this->HandleMouseHover(false);
						break;
					case ClientMessage:
//						TRACE(<< "ClientMessage X event got" << std::endl)
						//probably a WM_DELETE_WINDOW event
						{
							char* name = XGetAtomName(this->xDisplay.d, event.xclient.message_type);
							if(*name == *"WM_PROTOCOLS"){
								quitFlag = true;
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
		
		this->Render();
	}//~while(!quitFlag)
	
	waitSet.Remove(xew);
}



void App::ShowVirtualKeyboard()throw(){
	TRACE(<< "App::ShowVirtualKeyboard(): invoked" << std::endl)
	//do nothing
}



void App::HideVirtualKeyboard()throw(){
	TRACE(<< "App::HideVirtualKeyboard(): invoked" << std::endl)
	//do nothing
}



namespace morda{

inline void Main(int argc, const char** argv){
	ting::Ptr<morda::App> app = morda::CreateApp(argc, argv, ting::Buffer<const ting::u8>(0, 0));

	app->Exec();
}

}//~namespace



int main(int argc, const char** argv){
	morda::Main(argc, argv);

	return 0;
}
