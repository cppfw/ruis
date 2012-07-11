#include "Application.hpp"


#include <ting/debug.hpp>


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
		
		XMapWindow(this->xDisplay.d, this->window);
	}
	glXMakeCurrent(this->xDisplay.d, this->window, this->glxContext);
}



Application::~Application()throw(){
	glXMakeCurrent(this->xDisplay.d, None, NULL);
	glXDestroyContext(this->xDisplay.d, this->glxContext);
}



void Application::Exec(){
	//TODO:
}
