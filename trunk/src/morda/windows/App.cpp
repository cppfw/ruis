//This file contains implementations of platform dependent methods from App class.

#include "../App.hpp"

#include <windows.h>

#include <ting/fs/FSFile.hpp>


using namespace morda;



namespace morda{

bool HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& lres){
	morda::App& app = morda::App::Inst();

	switch(msg){
		case WM_ACTIVATE:
			if (!HIWORD(wParam)){ //Check Minimization State
				//window active
			}else{
				//window is no longer active
			}
			lres = 0;
			return true;

		case WM_SYSCOMMAND:
			switch(wParam){
				case SC_SCREENSAVE: //screensaver trying to start?
				case SC_MONITORPOWER: //montor trying to enter powersave?
					return 0; //prevent from happening
			}
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			lres = 0;
			return true;

		case WM_KEYDOWN:
			//TODO:
			//keys[wParam] = TRUE;					// If So, Mark It As TRUE
			lres = 0;
			return true;

		case WM_KEYUP:
			//TODO:
			//keys[wParam] = FALSE;					// If So, Mark It As FALSE
			lres = 0;
			return true;
		
		case WM_PAINT:
			app.Render();
			lres = 0;
			return true;

		case WM_SIZE:
			//TODO: resize GL
			//ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			lres = 0;
			return true;
			
		default:
			break;
	}
	return false;
}

}//~namespace



namespace{


LRESULT	CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	LRESULT res;
	if(morda::HandleWindowMessage(hwnd, msg, wParam, lParam, res)){
		return res;
	}
	
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

}//~namespace



App::WindowClassWrapper::WindowClassWrapper(){
	this->name = "MordaWindowClassName";
	
	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;//Redraw on resize, own DC for window
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.cbClsExtra = 0;//no extra window data
	wc.cbWndExtra = 0;//no extra window data
	wc.hInstance = GetModuleHandle(NULL);// instance handle
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;//no background required for OpenGL
	wc.lpszMenuName = NULL;//we don't want a menu
	wc.lpszClassName = this->name.c_str();// Set the window class Name
	
	if(!RegisterClass(&wc)){
		throw morda::Exc("Failed to register window class");
	}
}



App::WindowClassWrapper::~WindowClassWrapper()throw(){
	if(!UnregisterClass(this->name.c_str(), GetModuleHandle(NULL))){
		ASSERT_INFO(false, "Failed to unregister window class")
	}
}



App::WindowWrapper::WindowWrapper(const App::WindowParams& wp, const WindowClassWrapper& wc){
//	TRACE_AND_LOG(<< "App::WindowWrapper::WindowWrapper(): enter" << std::endl)
	this->hwnd = CreateWindowEx(
			WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, //extended style
			wc.name.c_str(),
			"morda app",
			WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0, //x
			0, //y
			wp.dim.x,
			wp.dim.y,
			NULL, //no parent window
			NULL, //no menu
			GetModuleHandle(NULL),
			NULL //do not pass anything to WM_CREATE
		);
	
	if(!this->hwnd){
		throw morda::Exc("Failed to create a window");
	}
	
	ShowWindow(this->hwnd, SW_SHOW);

//	TRACE_AND_LOG(<< "App::WindowWrapper::WindowWrapper(): exit" << std::endl)
}



App::WindowWrapper::~WindowWrapper()throw(){
	if(!DestroyWindow(this->hwnd)){
		ASSERT_INFO(false, "Failed to destroy window")
	}
}



App::DeviceContextWrapper::DeviceContextWrapper(const WindowParams& wp, const WindowWrapper& w) :
		w(w)
{
//	TRACE_AND_LOG(<< "App::DeviceContextWrapper::DeviceContextWrapper(): enter" << std::endl)

	this->hdc = GetDC(this->w.hwnd);
	if(!this->hdc){
		throw morda::Exc("Failed to create a OpenGL device context");
	}

//	TRACE_AND_LOG(<< "App::DeviceContextWrapper::DeviceContextWrapper(): DC created" << std::endl)

	//TODO: make pixel format configurable via WindowParams
	static	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1, // Version number of the structure, should be 1
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32, //color depth
		0, 0, 0, 0, 0, 0, //color bits ignored
		0, //no alpha buffer
		0, //shift bit ignored
		0, //no accumulation buffer
		0, 0, 0, 0, //accumulation bits ignored
		16, //16bit depth buffer
		0, //no stencil buffer
		0, //no auxiliary buffer
		PFD_MAIN_PLANE, //main drawing layer
		0, //reserved
		0, 0, 0 //layer masks ignored
	};
	
	GLuint pixelFormat = ChoosePixelFormat(this->hdc, &pfd);
	if(!pixelFormat){
		this->Destroy();
		throw morda::Exc("Could not find suitable pixel format");
	}

//	TRACE_AND_LOG(<< "App::DeviceContextWrapper::DeviceContextWrapper(): pixel format chosen" << std::endl)

	if(!SetPixelFormat(this->hdc, pixelFormat, &pfd)){
		this->Destroy();
		throw morda::Exc("Could not sent pixel format");
	}

//	TRACE_AND_LOG(<< "App::DeviceContextWrapper::DeviceContextWrapper(): pixel format set" << std::endl)
}



void App::DeviceContextWrapper::Destroy()throw(){
	if(!ReleaseDC(this->w.hwnd, this->hdc)){
		ASSERT_INFO(false, "Failed to release device context")
	}
}



App::GLContextWrapper::GLContextWrapper(const DeviceContextWrapper& dc){
//	TRACE_AND_LOG(<< "App::GLContextWrapper::GLContextWrapper(): enter" << std::endl)

	this->hrc = wglCreateContext(dc.hdc);
	if(!this->hrc){
		throw morda::Exc("Failed to create OpenGL rendering context");
	}

//	TRACE_AND_LOG(<< "App::GLContextWrapper::GLContextWrapper(): GL rendering context created" << std::endl)
	
	if(!wglMakeCurrent(dc.hdc, this->hrc)){
		this->Destroy();
		throw morda::Exc("Failed to activate OpenGL rendering context");
	}

//	TRACE_AND_LOG(<< "App::GLContextWrapper::GLContextWrapper(): GL rendering context created" << std::endl)
	
	TRACE_AND_LOG(<< "OpenGL version: " << glGetString(GL_VERSION) << std::endl)
	
	if(glewInit() != GLEW_OK){
		this->Destroy();
		throw morda::Exc("GLEW initialization failed");
	}
}



void App::GLContextWrapper::Destroy()throw(){
	if(!wglMakeCurrent(NULL, NULL)){
		ASSERT_INFO(false, "Deactivating OpenGL rendering context failed")
	}

	if(!wglDeleteContext(this->hrc)){
		ASSERT_INFO(false, "Releasing OpenGL rendering context failed")
	}
}



App::App(const WindowParams& requestedWindowParams) :
		window(requestedWindowParams, windowClass),
		deviceContext(requestedWindowParams, window),
		glContext(deviceContext),
		curWinRect(0, 0, -1, -1)
{
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



void App::ShowVirtualKeyboard()throw(){
	TRACE(<< "App::ShowVirtualKeyboard(): invoked" << std::endl)
	//do nothing
}



void App::HideVirtualKeyboard()throw(){
	TRACE(<< "App::HideVirtualKeyboard(): invoked" << std::endl)
	//do nothing
}



void App::Exec(){
	TRACE(<< "App::Exec(): enter" << std::endl)

	bool quitFlag = false;
	
	while(!quitFlag){
		DWORD status = MsgWaitForMultipleObjectsEx(
				0,
				NULL,
				this->updater.Update(),
				QS_ALLINPUT,
				MWMO_INPUTAVAILABLE
			);

		if(status == WAIT_OBJECT_0){
			MSG msg;
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
				if(msg.message == WM_QUIT){
					quitFlag = true;
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

/*
//TODO: remove
DWORD dwStart = GetTickCount();
 DWORD dwElapsed;
 while ((dwElapsed = GetTickCount() - dwStart) < dwTimeout) {
  DWORD dwStatus = MsgWaitForMultipleObjectsEx(0, NULL,
                    dwTimeout - dwElapsed, QS_ALLINPUT,
                    MWFMO_WAITANY | MWMO_INPUTAVAILABLE);
  if (dwStatus == WAIT_OBJECT_0) {
   MSG msg;
   while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
     PostQuitMessage((int)msg.wParam);
     return FALSE; // abandoned due to WM_QUIT
    }
    if (!CallMsgFilter(&msg, MSGF_SLEEPMSG)) {
     TranslateMessage(&msg);
     DispatchMessage(&msg);
    }
   }
  }//~if
 }
*/

}
