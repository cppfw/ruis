//This file contains implementations of platform dependent methods from App class.

#include <vector>

#include "../App.hpp"

#include <ting/WaitSet.hpp>
#include <ting/fs/FSFile.hpp>
#include <ting/utf8.hpp>



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



App::XVisualInfoWrapper::XVisualInfoWrapper(XDisplayWrapper& xDisplay){
	//TODO: allow configuring depth, stencil buffers
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



App::XWindowWrapper::XWindowWrapper(unsigned width, unsigned height, XDisplayWrapper& xDisplay, XVisualInfoWrapper& xVisualInfo) :
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
			StructureNotifyMask
		;

	this->w = XCreateWindow(
			this->d.d,
			RootWindow(this->d.d, xVisualInfo.vi->screen),
			0,
			0,
			width,
			height,
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
	char classname[] = "morda_app";
	
	this->xim = XOpenIM(this->d.d, NULL, classname, classname);
	if(this->xim == NULL){
		throw morda::Exc("XOpenIM() failed");
	}
	
	this->xic = XCreateIC(
			this->xim,
			XNClientWindow, this->w.w,
			XNFocusWindow, this->w.w,
			XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
			XNResourceName, classname,
			XNResourceClass, classname,
			NULL
		);
	if(this->xic == NULL){
		this->Destroy();
	}
	
	//TODO:
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


App::App(unsigned w, unsigned h) :
		xVisualInfo(xDisplay),
		xWindow(w, h, xDisplay, xVisualInfo),
		glxContex(xDisplay, xWindow, xVisualInfo),
		xInputMethod(xDisplay, xWindow),
		curWinRect(0, 0, -1, -1)
{
#ifdef DEBUG
	//print GLX version
	{
		int major, minor;
		glXQueryVersion(this->xDisplay.d, &major, &minor);
		TRACE(<< "GLX Version: " << major << "." << minor << std::endl)
	}
#endif
	
	this->UpdateWindowRect(morda::Rect2f(0, 0, float(w), float(h)));
}



ting::Ptr<ting::fs::File> App::CreateResourceFileInterface(const std::string& path)const{
	ting::Ptr<ting::fs::FSFile> fi = ting::fs::FSFile::New(path);
	fi->SetRootDir("res/");
	return fi;
}



namespace{

class XEventWaitable : public ting::Waitable{
	int fd;
	
	//override
	int GetHandle(){
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
		case 2:
			return Widget::MIDDLE;
		case 3:
			return Widget::RIGHT;
		case 4:
			return Widget::WHEEL_UP;
		case 5:
			return Widget::WHEEL_DOWN;
		default:
			return Widget::UNKNOWN;
	}
}



const key::Key keyCodeMap[ting::u8(-1) + 1] = {
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
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
	key::SPACE,
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
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::F11,//95
	key::F12,//96
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::RIGHT_CONTROL,//105
	key::SPACE,
	key::PRINT_SCREEN,//107
	key::RIGHT_ALT,//108
	key::SPACE,
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
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::PAUSE,//127
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::WINDOWS,//133
	key::SPACE,
	key::WINDOWS_MENU,//135
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE,
	key::SPACE
};



class KeyEventUnicodeResolver{
	XIC& xic;
	XEvent& event;
public:
	KeyEventUnicodeResolver(XIC& xic, XEvent& event) :
			xic(xic),
			event(event)
	{}
	
	ting::Array<ting::u32> Resolve(){
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
	
	waitSet.Add(&xew, ting::Waitable::READ);
//	waitSet.Add(&this->queue, ting::Waitable::READ);
	
	//Sometimes the first Expose event does not come for some reason. It happens constantly in some systems and never happens on all the others.
	//So, render everything for the first time.
	this->Render();
	
	while(!this->quitFlag){
		waitSet.WaitWithTimeout(this->updater.Update());
		
//		if(this->queue.CanRead()){
//			while(ting::Ptr<ting::mt::Message> m = this->queue.PeekMsg()){
//				m->Handle();
//			}
//		}
		
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
						{
							KeyEventUnicodeResolver resolver(this->xInputMethod.xic, event);
							this->HandleKeyEvent<true, false, KeyEventUnicodeResolver>(keyCodeMap[ting::u8(event.xkey.keycode)], resolver);
						}
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
								
								KeyEventUnicodeResolver resolver(this->xInputMethod.xic, nev);
								
								this->HandleKeyEvent<true, true, KeyEventUnicodeResolver>(keyCodeMap[ting::u8(nev.xkey.keycode)], resolver);
								
								XNextEvent(this->xDisplay.d, &nev);//remove the key down event from queue
								break;
							}
						}

						{
							KeyEventUnicodeResolver resolver(this->xInputMethod.xic, event);
							this->HandleKeyEvent<false, false, KeyEventUnicodeResolver>(keyCodeMap[ting::u8(event.xkey.keycode)], resolver);
						}
						break;
					case ButtonPress:
//						TRACE(<< "ButtonPress X event got, button mask = " << event.xbutton.button << std::endl)
//						TRACE(<< "ButtonPress X event got, x, y = " << event.xbutton.x << ", " << event.xbutton.y << std::endl)
						if(this->rootContainer.IsNotValid()){
							break;
						}
						this->rootContainer->OnMouseButtonDown(
								morda::Vec2f(event.xbutton.x, this->curWinRect.d.y - float(event.xbutton.y) - 1.0f),
								ButtonNumberToEnum(event.xbutton.button),
								0
							);
						break;
					case ButtonRelease:
						if(this->rootContainer.IsNotValid()){
							break;
						}
						this->rootContainer->OnMouseButtonUp(
								morda::Vec2f(event.xbutton.x, this->curWinRect.d.y - float(event.xbutton.y) - 1.0f),
								ButtonNumberToEnum(event.xbutton.button),
								0
							);
						break;
					case MotionNotify:
//						TRACE(<< "MotionNotify X event got" << std::endl)
						if(this->rootContainer.IsNotValid()){
							break;
						}
						this->rootContainer->OnMouseMove(
								morda::Vec2f(event.xmotion.x, this->curWinRect.d.y - float(event.xmotion.y) - 1.0f),
								0
							);
						break;
					case ClientMessage:
//						TRACE(<< "ClientMessage X event got" << std::endl)
						//probably a WM_DELETE_WINDOW event
						if(*XGetAtomName(this->xDisplay.d, event.xclient.message_type) == *"WM_PROTOCOLS"){
							this->quitFlag = true;
						}
						break;
					default:
						//ignore
						break;
				}//~switch()
			}//~while()
		}//~if there are pending X events
		
		//TODO: render only if needed?
		this->Render();
	}//~while(!this->quitFlag)
	
//	waitSet.Remove(&this->queue);
	waitSet.Remove(&xew);
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

inline void Main(int argc, char** argv){
	ting::Ptr<morda::App> app = morda::CreateApp(argc, argv, ting::Buffer<const ting::u8>(0, 0));

	app->Exec();
}

}//~namespace



int main(int argc, char** argv){
	morda::Main(argc, argv);

	return 0;
}
