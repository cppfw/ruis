//This file contains implementations of platform dependent methods from App class.

#include "../App.hpp"

#include <windows.h>


using namespace morda;



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
}



App::WindowWrapper::~WindowWrapper()throw(){
	if(!DestroyWindow(this->hwnd)){
		ASSERT_INFO(false, "Failed to destroy window")
	}
}



App::DeviceContextWrapper::DeviceContextWrapper(const WindowWrapper& w, const App::WindowParams& wp) :
		w(w)
{
	//TODO:
}



App::DeviceContextWrapper::~DeviceContextWrapper()throw(){
	if(!ReleaseDC(this->w.hwnd, this->hdc)){
		ASSERT_INFO(false, "Failed to release device context")
	}
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

