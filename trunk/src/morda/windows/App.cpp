//This file contains implementations of platform dependent methods from App class.

#include "../App.hpp"

#include <windows.h>


using namespace morda;



namespace{

LRESULT	CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_ACTIVATE:
			if (!HIWORD(wParam)){ //Check Minimization State
				//window active
			}else{
				//window is no longer active
			}
			return 0;

		case WM_SYSCOMMAND:
			switch(wParam){
				case SC_SCREENSAVE: //screensaver trying to start?
				case SC_MONITORPOWER: //montor trying to enter powersave?
					return 0; //prevent from happening
			}
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_KEYDOWN:
			//TODO:
			//keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;

		case WM_KEYUP:
			//TODO:
			//keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;

		case WM_SIZE:
			//TODO: resize GL
			//ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;
			
		default:
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
}

}//~namespace



App::WindowClassWrapper::WindowClassWrapper(){
	this->name = "MordaWindowClassName";
	
	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;//Redraw on resize, own DC for window
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.cbClsExtra = 0;//no extra windw data
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
	this->hWnd = CreateWindowEx(
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
}



App::WindowWrapper::~WindowWrapper()throw(){
	if(!DestroyWindow(this->hwnd)){
		ASSERT_INFO(false, "Failed to destroy window")
	}
}



App::DeviceContextWrapper::DeviceContextWrapper(const WindowParams& wp, const WindowWrapper& w) :
		w(w)
{
	this->hdc = GetDC(this->w.hwnd);
	if(!this->hdc){
		throw morda::Exc("Failed to create a OpenGL device context");
	}

	//TODO: make pixel format configurable via WindowParams
	static	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1, // Version number
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

	if(!SetPixelFormat(this->hdc, pixelFormat, &pfd)){
		this->Destroy();
		throw morda::Exc("Could not sent pixel format");
	}
}



App::DeviceContextWrapper::Destroy()throw(){
	if(!ReleaseDC(this->w.hwnd, this->hdc)){
		ASSERT_INFO(false, "Failed to release device context")
	}
}



App::GLContextWrapper::GLContextWrapper(const DeviceContextWrapper& dc){
	this->hrc = wglCreateContext(dc.hdc);
	if(!this->hrc){
		throw morda::Exc("Failed to create OpenGL rendering context");
	}
	
	if(!wglMakeCurrent(dc.hdc, this->hrc)){
		this->Destroy();
		throw morda::Exc("Failed to activate OpenGL rendering context");
	}
	
	TRACE(<< "OpenGL version: " << glGetString(GL_VERSION) << std::endl)
	
	if(glewInit() != GLEW_OK){
		this->Destroy();
		throw morda::Exc("GLEW initialization failed");
	}
}



App::GLContextWrapper::Destroy()throw(){
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
		glContext(deviceContext)
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



int WINAPI WinMain(
		HINSTANCE hInstance, // Instance
		HINSTANCE hPrevInstance, // Previous Instance
		LPSTR lpCmdLine, // Command Line Parameters
		int nCmdShow // Window Show State
	)
{
	//TODO: create App object
	
	
	MSG msg; // Windows Message Structure
	BOOL done = FALSE; // Bool Variable To Exit Loop

	//TODO: remove
	//Ask The User Which Screen Mode They Prefer
//	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
//	{
//		fullscreen=FALSE;							// Windowed Mode
//	}

	if(!CreateGLWindow("NeHe's OpenGL Framework",640,480,16,fullscreen)){
		return 0;									// Quit If Window Was Not Created
	}

	while(!done){
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active)								// Program Active?
			{
				if (keys[VK_ESCAPE])				// Was ESC Pressed?
				{
					done=TRUE;						// ESC Signalled A Quit
				}
				else								// Not Time To Quit, Update Screen
				{
					DrawGLScene();					// Draw The Scene
					SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
				}
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("NeHe's OpenGL Framework",640,480,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}

