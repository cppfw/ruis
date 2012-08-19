#include "App.hpp"


#include <ting/debug.hpp>
#include <ting/WaitSet.hpp>



using namespace morda;



ting::IntrusiveSingleton<App>::T_Instance App::instance;



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
	int attr[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24, //TODO: allow configuring depth, stencil buffers
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
	glXMakeCurrent(this->d.d, this->w.w, NULL);
	glXDestroyContext(this->d.d, this->glxContext);
}



App::App(unsigned w, unsigned h) :
		xVisualInfo(xDisplay),
		xWindow(w, h, xDisplay, xVisualInfo),
		glxContex(xDisplay, xWindow, xVisualInfo)
{	
#ifdef DEBUG
	//print GLX version
	{
		int major, minor;
		glXQueryVersion(this->xDisplay.d, &major, &minor);
		TRACE(<< "GLX Version: " << major << "." << minor << std::endl)
	}
#endif
	
	this->curWinDim.x = float(w);
	this->curWinDim.y = float(h);
	this->SetGLViewport(this->curWinDim);
}



void App::SetGLViewport(const tride::Vec2f& dim){
	glViewport(0, 0, dim.x, dim.y);
}



void App::Render(){
	if(this->rootContainer.IsNotValid()){
		return;
	}
	
	ASSERT(glGetError() == GL_NO_ERROR)

	glClearColor(0.0f, 0, 0.0f, 1.0f);
	ASSERT(glGetError() == GL_NO_ERROR)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ASSERT(glGetError() == GL_NO_ERROR)
	
	tride::Matr4f m;
	m.Identity();
	m.Translate(-1, -1);
	m.Scale(2.0f / this->curWinDim.x, 2.0f / this->curWinDim.y);
	
	this->rootContainer->Render(m);
	
	glXSwapBuffers(this->xDisplay.d, this->xWindow.w);
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

}//~namespace



void App::Exec(){
	
	XEventWaitable xew(this->xDisplay.d);
	
	ting::WaitSet waitSet(2);
	
	waitSet.Add(&xew, ting::Waitable::READ);
	//TODO: add queue?
	
	
	
	while(!this->quitFlag){
		waitSet.Wait();
		
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
						if(
								this->curWinDim.x != float(event.xconfigure.width) ||
								this->curWinDim.y != float(event.xconfigure.height)
							)
						{
							this->curWinDim.x = float(event.xconfigure.width);
							this->curWinDim.y = float(event.xconfigure.height);
							
							this->SetGLViewport(this->curWinDim);
							if(this->rootContainer.IsValid()){
								this->rootContainer->Resize(this->curWinDim);
							}
						}
						break;
					case KeyPress:
//						TRACE(<< "KeyPress X event got" << std::endl)
						//TODO:
						break;
					case KeyRelease:
//						TRACE(<< "KeyRelease X event got" << std::endl)
						//TODO:
						break;
					case ButtonPress:
//						TRACE(<< "ButtonPress X event got, button mask = " << event.xbutton.button << std::endl)
//						TRACE(<< "ButtonPress X event got, x, y = " << event.xbutton.x << ", " << event.xbutton.y << std::endl)
						if(this->rootContainer.IsValid()){
							this->rootContainer->OnMouseButtonDown(
									tride::Vec2f(event.xbutton.x, this->curWinDim.y - float(event.xbutton.y) - 1.0f),
									ButtonNumberToEnum(event.xbutton.button),
									0
								);
						}
						break;
					case ButtonRelease:
						if(this->rootContainer.IsValid()){
							this->rootContainer->OnMouseButtonUp(
									tride::Vec2f(event.xbutton.x, this->curWinDim.y - float(event.xbutton.y) - 1.0f),
									ButtonNumberToEnum(event.xbutton.button),
									0
								);
						}
						break;
					case MotionNotify:
//						TRACE(<< "MotionNotify X event got" << std::endl)
						if(this->rootContainer.IsValid()){
							this->rootContainer->OnMouseMove(
									tride::Vec2f(event.xmotion.x, this->curWinDim.y - float(event.xmotion.y) - 1.0f),
									0
								);
						}
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
		
		//TODO:
		
		this->Render();
	}//~while(!this->quitFlag)
	
	waitSet.Remove(&xew);
}
