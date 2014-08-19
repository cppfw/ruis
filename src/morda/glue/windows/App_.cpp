//This file contains implementations of platform dependent methods from App class.

#include "../../App.hpp"

#include <windows.h>
#include <windowsx.h>

#include <ting/fs/FSFile.hpp>


using namespace morda;



namespace{

const key::Key keyCodeMap[std::uint8_t(-1) + 1] = {
	key::UNKNOWN, //Undefined
	key::UNKNOWN, //VK_LBUTTON
	key::UNKNOWN, //VK_RBUTTON
	key::UNKNOWN, //VK_CANCEL
	key::UNKNOWN, //VK_MBUTTON
	key::UNKNOWN, //VK_XBUTTON1, 5
	key::UNKNOWN, //VK_XBUTTON2
	key::UNKNOWN, //Undefined
	key::BACKSPACE, //VK_BACK = backspace key
	key::TAB, //VK_TAB
	key::UNKNOWN, //Reserved, 10
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //VK_CLEAR = clear key???
	key::ENTER, //VK_RETURN
	key::UNKNOWN, //Undefined
	key::UNKNOWN, //Undefined, 15
	key::LEFT_SHIFT, //VK_SHIFT
	key::LEFT_CONTROL, //VK_CONTROL
	key::LEFT_ALT, //VK_MENU = alt key
	key::PAUSE, //VK_PAUSE
	key::CAPSLOCK, //VK_CAPITAL = caps lock key, 20
	key::UNKNOWN, //VK_KANA, VK_HANGUEL, VK_HANGUL
	key::UNKNOWN, //Undefined
	key::UNKNOWN, //VK_JUNJA
	key::UNKNOWN, //VK_FINAL
	key::UNKNOWN, //VK_HANJA, VK_KANJI, 25
	key::UNKNOWN, //Undefined
	key::ESCAPE, //VK_ESCAPE
	key::UNKNOWN, //VK_CONVERT
	key::UNKNOWN, //VK_NONCONVERT
	key::UNKNOWN, //VK_ACCEPT, 30
	key::UNKNOWN, //VK_MODECHANGE
	key::SPACE, //VK_SPACE = space bar key
	key::PAGE_UP, //VK_PRIOR = page up key
	key::PAGE_DOWN,//VK_NEXT = page down key
	key::END, //VK_END, 35
	key::HOME,//VK_HOME
	key::LEFT,//VK_LEFT
	key::UP, //VK_UP
	key::RIGHT, //VK_RIGHT
	key::DOWN, //VK_DOWN, 40
	key::UNKNOWN, //VK_SELECT
	key::UNKNOWN, //VK_PRINT
	key::UNKNOWN, //VK_EXECUTE
	key::PRINT_SCREEN,//VK_SNAPSHOT = print screen key
	key::INSERT, //VK_INSERT, 45
	key::DELETE, //VK_DELETE
	key::UNKNOWN, //VK_HELP
	key::ZERO, //0 key
	key::ONE, //1 key
	key::TWO, //2 key, 50
	key::THREE, //3 key
	key::FOUR, //4 key
	key::FIVE, //5 key
	key::SIX, //6 key
	key::SEVEN, //7 key, 55
	key::EIGHT, //8 key
	key::NINE, //9 key
	key::UNKNOWN, //Undefined
	key::UNKNOWN, //Undefined
	key::UNKNOWN, //Undefined, 60
	key::UNKNOWN, //Undefined
	key::UNKNOWN, //Undefined
	key::UNKNOWN, //Undefined
	key::UNKNOWN, //Undefined
	key::A, //A key, 65
	key::B, //B key
	key::C, //C key
	key::D, //D key
	key::E, //E key
	key::F, //F key, 70
	key::G, //G key
	key::H, //H key
	key::I, //I key
	key::J, //J key
	key::K, //K key, 75
	key::L, //L key
	key::M, //M key
	key::N, //N key
	key::O, //O key
	key::P, //P key, 80
	key::Q, //Q key
	key::R, //R key
	key::S, //S key
	key::T, //T key
	key::U, //U key, 85
	key::V, //V key
	key::W, //W key
	key::X, //X key
	key::Y, //Y key
	key::Z, //Z key, 90
	key::WINDOWS, //VK_LWIN = left windows key
	key::WINDOWS, //VK_RWIN = right windows key
	key::UNKNOWN, //VK_APPS = applications key
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //VK_SLEEP = computer sleep key, 95
	key::ZERO, //VK_NUMPAD0 = numeric keypad 0 key
	key::ONE, //VK_NUMPAD1 = numeric keypad 1 key
	key::TWO, //VK_NUMPAD2 = numeric keypad 2 key
	key::THREE, //VK_NUMPAD3 = numeric keypad 3 key
	key::FOUR, //VK_NUMPAD4 = numeric keypad 4 key, 100
	key::FIVE, //VK_NUMPAD5 = numeric keypad 5 key
	key::SIX, //VK_NUMPAD6 = numeric keypad 6 key
	key::SEVEN, //VK_NUMPAD7 = numeric keypad 7 key
	key::EIGHT, //VK_NUMPAD8 = numeric keypad 8 key
	key::NINE, //VK_NUMPAD9 = numeric keypad 9 key, 105
	key::UNKNOWN, //VK_MULTIPLY = multiply key
	key::UNKNOWN, //VK_ADD
	key::UNKNOWN, //VK_SEPARATOR
	key::UNKNOWN, //VK_SUBTRACT
	key::UNKNOWN, //VK_DECIMAL, 110
	key::UNKNOWN, //VK_DIVIDE
	key::F1, //VK_F1
	key::F2, //VK_F2
	key::F3, //VK_F3
	key::F4, //VK_F4, 115
	key::F5, //VK_F5
	key::F6, //VK_F6
	key::F7, //VK_F7
	key::F8, //VK_F8
	key::F9, //VK_F9, 120
	key::F10, //VK_F10
	key::F11, //VK_F11
	key::F12, //VK_F12
	key::UNKNOWN, //VK_F13
	key::UNKNOWN, //VK_F14, 125
	key::UNKNOWN, //VK_F15
	key::UNKNOWN, //VK_F16
	key::UNKNOWN, //VK_F17
	key::UNKNOWN, //VK_F18
	key::UNKNOWN, //VK_F19, 130
	key::UNKNOWN, //VK_F20
	key::UNKNOWN, //VK_F21
	key::UNKNOWN, //VK_F22
	key::UNKNOWN, //VK_F23
	key::UNKNOWN, //VK_F24, 135
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned, 140
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //VK_NUMLOCK
	key::UNKNOWN, //VK_SCROLL = scroll lock key, 145
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific, 150
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned, 155
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::LEFT_SHIFT, //VK_LSHIFT, 160
	key::RIGHT_SHIFT, //VK_RSHIFT
	key::LEFT_CONTROL, //VK_LCONTROL
	key::RIGHT_CONTROL, //VK_RCONTROL
	key::WINDOWS_MENU, //VK_LMENU = left menu key
	key::WINDOWS_MENU, //VK_RMENU, 165
	key::UNKNOWN, //VK_BROWSER_BACK
	key::UNKNOWN, //VK_BROWSER_FORWARD
	key::UNKNOWN, //VK_BROWSER_REFRESH
	key::UNKNOWN, //VK_BROWSER_STOP
	key::UNKNOWN, //VK_BROWSER_SEARCH, 170
	key::UNKNOWN, //VK_BROWSER_FAVORITES
	key::UNKNOWN, //VK_BROWSER_HOME
	key::UNKNOWN, //VK_VOLUME_MUTE
	key::UNKNOWN, //VK_VOLUME_DOWN
	key::UNKNOWN, //VK_VOLUME_UP, 175
	key::UNKNOWN, //VK_MEDIA_NEXT_TRACK
	key::UNKNOWN, //VK_MEDIA_PREV_TRACK
	key::UNKNOWN, //VK_MEDIA_STOP
	key::UNKNOWN, //VK_MEDIA_PLAY_PAUSE
	key::UNKNOWN, //VK_LAUNCH_MAIL, 180
	key::UNKNOWN, //VK_LAUNCH_MEDIA_SELECT
	key::UNKNOWN, //VK_LAUNCH_APP1
	key::UNKNOWN, //VK_LAUNCH_APP2
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved, 185
	key::UNKNOWN, //VK_OEM_1
	key::UNKNOWN, //VK_OEM_PLUS
	key::UNKNOWN, //VK_OEM_COMMA
	key::UNKNOWN, //VK_OEM_MINUS
	key::UNKNOWN, //VK_OEM_PERIOD, 190
	key::UNKNOWN, //VK_OEM_2
	key::UNKNOWN, //VK_OEM_3
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved, 195
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved, 200
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved, 205
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved, 210
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //Reserved, 215
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //VK_OEM_4
	key::UNKNOWN, //VK_OEM_5, 220
	key::UNKNOWN, //VK_OEM_6
	key::UNKNOWN, //VK_OEM_7
	key::UNKNOWN, //VK_OEM_8
	key::UNKNOWN, //Reserved
	key::UNKNOWN, //OEM specific, 225
	key::UNKNOWN, //VK_OEM_102
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //VK_PROCESSKEY
	key::UNKNOWN, //OEM specific, 230
	key::UNKNOWN, //VK_PACKET
	key::UNKNOWN, //Unassigned
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific, 235
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific, 240
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific
	key::UNKNOWN, //OEM specific, 245
	key::UNKNOWN, //VK_ATTN
	key::UNKNOWN, //VK_CRSEL
	key::UNKNOWN, //VK_EXSEL
	key::UNKNOWN, //VK_EREOF
	key::UNKNOWN, //VK_PLAY, 250
	key::UNKNOWN, //VK_ZOOM
	key::UNKNOWN, //VK_NONAME
	key::UNKNOWN, //VK_PA1
	key::UNKNOWN, //VK_OEM_CLEAR
	key::UNKNOWN
};

class KeyEventUnicodeResolver{
	std::uint32_t c;
public:
	KeyEventUnicodeResolver(std::uint32_t unicodeChar) :
			c(unicodeChar)
	{}
	
	std::vector<std::uint32_t> Resolve()const{
		std::vector<std::uint32_t> ret(1);
		ret[0] = this->c;
		return ret;
	}
};



}//~namespace



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

		case WM_MOUSEMOVE:
			app.HandleMouseMove(
					morda::Vec2f(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					0
				);
			lres = 0;
			return true;

		case WM_LBUTTONDOWN:
			app.HandleMouseButton(
					true,
					morda::Vec2f(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					Widget::LEFT,
					0
				);
			lres = 0;
			return true;

		case WM_LBUTTONUP:
			app.HandleMouseButton(
					false,
					morda::Vec2f(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					Widget::LEFT,
					0
				);
			lres = 0;
			return true;

		case WM_MBUTTONDOWN:
			app.HandleMouseButton(
					true,
					morda::Vec2f(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					Widget::MIDDLE,
					0
				);
			lres = 0;
			return true;

		case WM_MBUTTONUP:
			app.HandleMouseButton(
					false,
					morda::Vec2f(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					Widget::MIDDLE,
					0
				);
			lres = 0;
			return true;

		case WM_RBUTTONDOWN:
			app.HandleMouseButton(
					true,
					morda::Vec2f(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					Widget::RIGHT,
					0
				);
			lres = 0;
			return true;

		case WM_RBUTTONUP:
			app.HandleMouseButton(
					false,
					morda::Vec2f(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					Widget::RIGHT,
					0
				);
			lres = 0;
			return true;

		case WM_MOUSEWHEEL:
			//TODO:
			lres = 0;
			return true;

		case WM_KEYDOWN:
			if((lParam & 0x40000000) == 0){//ignore auto-repeated keypress event
				app.HandleKeyEvent(true, keyCodeMap[std::uint8_t(wParam)]);
			}
			lres = 0;
			return true;

		case WM_KEYUP:
			app.HandleKeyEvent(false, keyCodeMap[std::uint8_t(wParam)]);
			lres = 0;
			return true;
		
		case WM_CHAR:
			app.HandleCharacterInput(KeyEventUnicodeResolver(wParam));
			lres = 0;
			return true;
		case WM_PAINT:
			//we will redraw anyway on every cycle
			//app.Render();
			ValidateRect(hwnd, NULL);//This is to tell Windows that we have redrawn contents and WM_PAINT should go away from message queue.
			lres = 0;
			return true;

		case WM_SIZE:
			//resize GL, LoWord=Width, HiWord=Height
			app.UpdateWindowRect(morda::Rect2f(0, 0, float(LOWORD(lParam)), float(HIWORD(lParam)))); 
			lres = 0;
			return true;
		
		case WM_USER:
			{
				std::unique_ptr<std::function<void()>> m(reinterpret_cast<std::function<void()>*>(lParam));
				(*m)();
			}
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



App::WindowClassWrapper::~WindowClassWrapper()NOEXCEPT{
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
			wp.dim.x + 2 * GetSystemMetrics(SM_CXSIZEFRAME),
			wp.dim.y + GetSystemMetrics(SM_CYCAPTION) + 2 * GetSystemMetrics(SM_CYSIZEFRAME),
			NULL, //no parent window
			NULL, //no menu
			GetModuleHandle(NULL),
			NULL //do not pass anything to WM_CREATE
		);
	
	if(!this->hwnd){
		throw morda::Exc("Failed to create a window");
	}
	
	//NOTE: window will be shown later, right before entering main loop and after all initial App data is initialized

//	TRACE_AND_LOG(<< "App::WindowWrapper::WindowWrapper(): exit" << std::endl)
}



App::WindowWrapper::~WindowWrapper()NOEXCEPT{
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



void App::DeviceContextWrapper::Destroy()NOEXCEPT{
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



void App::GLContextWrapper::Destroy()NOEXCEPT{
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
	this->dotsPerCm = (float(GetDeviceCaps(this->deviceContext.hdc, HORZRES)) * 10.0f / float(GetDeviceCaps(this->deviceContext.hdc, HORZSIZE))
			+ float(GetDeviceCaps(this->deviceContext.hdc, VERTRES)) * 10.0f / float(GetDeviceCaps(this->deviceContext.hdc, VERTSIZE))) / 2.0f;
	
	this->UpdateWindowRect(
			morda::Rect2f(
					0,
					0,
					float(requestedWindowParams.dim.x),
					float(requestedWindowParams.dim.y)
				)
		);
}



void App::PostToUIThread_ts(std::function<void()>&& msg){
	if(PostMessage(this->window.hwnd, WM_USER, 0, reinterpret_cast<LPARAM>(new std::remove_reference<decltype(msg)>::type(std::move(msg)))) == 0){
		throw morda::Exc("PostMessage(): failed");
	}
}



void App::Exec(){
//	TRACE(<< "App::Exec(): enter" << std::endl)

	bool quitFlag = false;
	
	while(!this->quitFlag){
		std::uint32_t timeout = this->updater.Update();
//		TRACE(<< "timeout = " << timeout << std::endl)

		DWORD status = MsgWaitForMultipleObjectsEx(
				0,
				NULL,
				timeout,
				QS_ALLINPUT,
				MWMO_INPUTAVAILABLE
			);

//		TRACE(<< "msg" << std::endl)

		if(status == WAIT_OBJECT_0){
			MSG msg;
			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
//				TRACE(<< "msg got, msg.message = " << msg.message << std::endl)
				if(msg.message == WM_QUIT){
					this->quitFlag = true;
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		this->Render();
//		TRACE(<< "loop" << std::endl)
	}
}
