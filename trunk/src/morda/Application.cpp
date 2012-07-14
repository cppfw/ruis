#include "Application.hpp"


#include <ting/debug.hpp>
#include <ting/WaitSet.hpp>



using namespace morda;



Application::XDisplayWrapper::XDisplayWrapper(){
	this->d = XOpenDisplay(0);
	if(!this->d){
		throw morda::Exc("XOpenDisplay() failed");
	}
}



Application::XDisplayWrapper::~XDisplayWrapper()throw(){
	XCloseDisplay(this->d);
}



Application::Application(unsigned w, unsigned h){
	int screen = DefaultScreen(this->xDisplay.d);
	
	XVisualInfo *vi;
	{
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

		vi = glXChooseVisual(this->xDisplay.d, screen, attr);
		if(!vi){
			throw morda::Exc("glXChooseVisual() failed");
		}
	}
	
#ifdef DEBUG
	//print GLX version
	{
		int major, minor;
		glXQueryVersion(this->xDisplay.d, &major, &minor);
		TRACE(<< "GLX Version: " << major << "." << minor << std::endl)
	}
#endif
	
	this->glxContext = glXCreateContext(this->xDisplay.d, vi, 0, GL_TRUE);
	if(this->glxContext == NULL){
		throw morda::Exc("glXCreateContext() failed");
	}
	
	{
		Colormap colorMap = XCreateColormap(
				this->xDisplay.d,
				RootWindow(this->xDisplay.d, vi->screen),
				vi->visual,
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
		
		this->window = XCreateWindow(
				this->xDisplay.d,
				RootWindow(this->xDisplay.d, vi->screen),
				0,
				0,
				w,
				h,
				0,
				vi->depth,
				InputOutput,
				vi->visual,
				CWBorderPixel | CWColormap | CWEventMask,
				&attr
			);
		//TODO: check for error
		
		{//We want to handle WM_DELETE_WINDOW event to know when window is closed.
			Atom a = XInternAtom(this->xDisplay.d, "WM_DELETE_WINDOW", True);
			XSetWMProtocols(this->xDisplay.d, this->window, &a, 1);
		}
		
		XMapWindow(this->xDisplay.d, this->window);
	}
	glXMakeCurrent(this->xDisplay.d, this->window, this->glxContext);
	
	TRACE(<< "OpenGL version: " << glGetString(GL_VERSION) << std::endl)
	
	if(glewInit() != GLEW_OK){
		throw morda::Exc("GLEW initialization failed");
	}
	
	this->curWinDim.x = float(w);
	this->curWinDim.y = float(h);
	this->SetGLViewport(this->curWinDim);
}



Application::~Application()throw(){
	glXMakeCurrent(this->xDisplay.d, this->window, NULL);
	glXDestroyContext(this->xDisplay.d, this->glxContext);
	XDestroyWindow(this->xDisplay.d, this->window);
}



void Application::SetGLViewport(const tride::Vec2f& dim){
	glViewport(0, 0, dim.x, dim.y);
}



void Application::Render(){
	if(this->rootWidget.IsNotValid()){
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
	
	this->rootWidget->Render(m);
	
	glXSwapBuffers(this->xDisplay.d, this->window);
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

}//~namespace



void Application::Exec(){
	
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
							if(this->rootWidget.IsValid()){
								this->rootWidget->Resize(this->curWinDim);
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
						if(this->rootWidget.IsValid()){
							Widget::EMouseButton b;
							switch(event.xbutton.button){
								case 1:
									b = Widget::LEFT;
									break;
								case 2:
									b = Widget::MIDDLE;
									break;
								case 3:
									b = Widget::RIGHT;
									break;
								case 4:
									b = Widget::WHEEL_UP;
									break;
								case 5:
									b = Widget::WHEEL_DOWN;
									break;
								default:
									b = Widget::UNKNOWN;
									break;
							}
							this->rootWidget->OnMouseButtonDown(
									tride::Vec2f(event.xbutton.x, this->curWinDim.y - float(event.xbutton.y) - 1.0f),
									b,
									0
								);
						}
						break;
					case ButtonRelease:
//						TRACE(<< "ButtonRelease X event got" << std::endl)
						//TODO:
						break;
					case MotionNotify:
//						TRACE(<< "MotionNotify X event got" << std::endl)
						if(this->rootWidget.IsValid()){
							this->rootWidget->OnMouseMove(
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
	}
	
	waitSet.Remove(&xew);
}
