//This file contains implementations of platform dependent methods from App class.

#include "../../App.hpp"

#include <utki/windows.hpp>
#include <papki/FSFile.hpp>

#include <windowsx.h>


using namespace morda;


namespace{

	const std::array<morda::EKey, std::uint8_t(-1) + 1> keyCodeMap = {
		EKey::UNKNOWN, //Undefined
		EKey::UNKNOWN, //VK_LBUTTON
		EKey::UNKNOWN, //VK_RBUTTON
		EKey::UNKNOWN, //VK_CANCEL
		EKey::UNKNOWN, //VK_MBUTTON
		EKey::UNKNOWN, //VK_XBUTTON1, 5
		EKey::UNKNOWN, //VK_XBUTTON2
		EKey::UNKNOWN, //Undefined
		EKey::BACKSPACE, //VK_BACK = backspace key
		EKey::TAB, //VK_TAB
		EKey::UNKNOWN, //Reserved, 10
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //VK_CLEAR = clear key???
		EKey::ENTER, //VK_RETURN
		EKey::UNKNOWN, //Undefined
		EKey::UNKNOWN, //Undefined, 15
		EKey::LEFT_SHIFT, //VK_SHIFT
		EKey::LEFT_CONTROL, //VK_CONTROL
		EKey::LEFT_ALT, //VK_MENU = alt key
		EKey::PAUSE, //VK_PAUSE
		EKey::CAPSLOCK, //VK_CAPITAL = caps lock key, 20
		EKey::UNKNOWN, //VK_KANA, VK_HANGUEL, VK_HANGUL
		EKey::UNKNOWN, //Undefined
		EKey::UNKNOWN, //VK_JUNJA
		EKey::UNKNOWN, //VK_FINAL
		EKey::UNKNOWN, //VK_HANJA, VK_KANJI, 25
		EKey::UNKNOWN, //Undefined
		EKey::ESCAPE, //VK_ESCAPE
		EKey::UNKNOWN, //VK_CONVERT
		EKey::UNKNOWN, //VK_NONCONVERT
		EKey::UNKNOWN, //VK_ACCEPT, 30
		EKey::UNKNOWN, //VK_MODECHANGE
		EKey::SPACE, //VK_SPACE = space bar key
		EKey::PAGE_UP, //VK_PRIOR = page up key
		EKey::PAGE_DOWN,//VK_NEXT = page down key
		EKey::END, //VK_END, 35
		EKey::HOME,//VK_HOME
		EKey::LEFT,//VK_LEFT
		EKey::UP, //VK_UP
		EKey::RIGHT, //VK_RIGHT
		EKey::DOWN, //VK_DOWN, 40
		EKey::UNKNOWN, //VK_SELECT
		EKey::UNKNOWN, //VK_PRINT
		EKey::UNKNOWN, //VK_EXECUTE
		EKey::PRINT_SCREEN,//VK_SNAPSHOT = print screen key
		EKey::INSERT, //VK_INSERT, 45
		EKey::DELETE, //VK_DELETE
		EKey::UNKNOWN, //VK_HELP
		EKey::ZERO, //0 key
		EKey::ONE, //1 key
		EKey::TWO, //2 key, 50
		EKey::THREE, //3 key
		EKey::FOUR, //4 key
		EKey::FIVE, //5 key
		EKey::SIX, //6 key
		EKey::SEVEN, //7 key, 55
		EKey::EIGHT, //8 key
		EKey::NINE, //9 key
		EKey::UNKNOWN, //Undefined
		EKey::UNKNOWN, //Undefined
		EKey::UNKNOWN, //Undefined, 60
		EKey::UNKNOWN, //Undefined
		EKey::UNKNOWN, //Undefined
		EKey::UNKNOWN, //Undefined
		EKey::UNKNOWN, //Undefined
		EKey::A, //A key, 65
		EKey::B, //B key
		EKey::C, //C key
		EKey::D, //D key
		EKey::E, //E key
		EKey::F, //F key, 70
		EKey::G, //G key
		EKey::H, //H key
		EKey::I, //I key
		EKey::J, //J key
		EKey::K, //K key, 75
		EKey::L, //L key
		EKey::M, //M key
		EKey::N, //N key
		EKey::O, //O key
		EKey::P, //P key, 80
		EKey::Q, //Q key
		EKey::R, //R key
		EKey::S, //S key
		EKey::T, //T key
		EKey::U, //U key, 85
		EKey::V, //V key
		EKey::W, //W key
		EKey::X, //X key
		EKey::Y, //Y key
		EKey::Z, //Z key, 90
		EKey::WINDOWS, //VK_LWIN = left windows key
		EKey::WINDOWS, //VK_RWIN = right windows key
		EKey::UNKNOWN, //VK_APPS = applications key
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //VK_SLEEP = computer sleep key, 95
		EKey::ZERO, //VK_NUMPAD0 = numeric keypad 0 key
		EKey::ONE, //VK_NUMPAD1 = numeric keypad 1 key
		EKey::TWO, //VK_NUMPAD2 = numeric keypad 2 key
		EKey::THREE, //VK_NUMPAD3 = numeric keypad 3 key
		EKey::FOUR, //VK_NUMPAD4 = numeric keypad 4 key, 100
		EKey::FIVE, //VK_NUMPAD5 = numeric keypad 5 key
		EKey::SIX, //VK_NUMPAD6 = numeric keypad 6 key
		EKey::SEVEN, //VK_NUMPAD7 = numeric keypad 7 key
		EKey::EIGHT, //VK_NUMPAD8 = numeric keypad 8 key
		EKey::NINE, //VK_NUMPAD9 = numeric keypad 9 key, 105
		EKey::UNKNOWN, //VK_MULTIPLY = multiply key
		EKey::UNKNOWN, //VK_ADD
		EKey::UNKNOWN, //VK_SEPARATOR
		EKey::UNKNOWN, //VK_SUBTRACT
		EKey::UNKNOWN, //VK_DECIMAL, 110
		EKey::UNKNOWN, //VK_DIVIDE
		EKey::F1, //VK_F1
		EKey::F2, //VK_F2
		EKey::F3, //VK_F3
		EKey::F4, //VK_F4, 115
		EKey::F5, //VK_F5
		EKey::F6, //VK_F6
		EKey::F7, //VK_F7
		EKey::F8, //VK_F8
		EKey::F9, //VK_F9, 120
		EKey::F10, //VK_F10
		EKey::F11, //VK_F11
		EKey::F12, //VK_F12
		EKey::UNKNOWN, //VK_F13
		EKey::UNKNOWN, //VK_F14, 125
		EKey::UNKNOWN, //VK_F15
		EKey::UNKNOWN, //VK_F16
		EKey::UNKNOWN, //VK_F17
		EKey::UNKNOWN, //VK_F18
		EKey::UNKNOWN, //VK_F19, 130
		EKey::UNKNOWN, //VK_F20
		EKey::UNKNOWN, //VK_F21
		EKey::UNKNOWN, //VK_F22
		EKey::UNKNOWN, //VK_F23
		EKey::UNKNOWN, //VK_F24, 135
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned, 140
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //VK_NUMLOCK
		EKey::UNKNOWN, //VK_SCROLL = scroll lock key, 145
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific, 150
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned, 155
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::LEFT_SHIFT, //VK_LSHIFT, 160
		EKey::RIGHT_SHIFT, //VK_RSHIFT
		EKey::LEFT_CONTROL, //VK_LCONTROL
		EKey::RIGHT_CONTROL, //VK_RCONTROL
		EKey::WINDOWS_MENU, //VK_LMENU = left menu key
		EKey::WINDOWS_MENU, //VK_RMENU, 165
		EKey::UNKNOWN, //VK_BROWSER_BACK
		EKey::UNKNOWN, //VK_BROWSER_FORWARD
		EKey::UNKNOWN, //VK_BROWSER_REFRESH
		EKey::UNKNOWN, //VK_BROWSER_STOP
		EKey::UNKNOWN, //VK_BROWSER_SEARCH, 170
		EKey::UNKNOWN, //VK_BROWSER_FAVORITES
		EKey::UNKNOWN, //VK_BROWSER_HOME
		EKey::UNKNOWN, //VK_VOLUME_MUTE
		EKey::UNKNOWN, //VK_VOLUME_DOWN
		EKey::UNKNOWN, //VK_VOLUME_UP, 175
		EKey::UNKNOWN, //VK_MEDIA_NEXT_TRACK
		EKey::UNKNOWN, //VK_MEDIA_PREV_TRACK
		EKey::UNKNOWN, //VK_MEDIA_STOP
		EKey::UNKNOWN, //VK_MEDIA_PLAY_PAUSE
		EKey::UNKNOWN, //VK_LAUNCH_MAIL, 180
		EKey::UNKNOWN, //VK_LAUNCH_MEDIA_SELECT
		EKey::UNKNOWN, //VK_LAUNCH_APP1
		EKey::UNKNOWN, //VK_LAUNCH_APP2
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved, 185
		EKey::UNKNOWN, //VK_OEM_1
		EKey::UNKNOWN, //VK_OEM_PLUS
		EKey::UNKNOWN, //VK_OEM_COMMA
		EKey::UNKNOWN, //VK_OEM_MINUS
		EKey::UNKNOWN, //VK_OEM_PERIOD, 190
		EKey::UNKNOWN, //VK_OEM_2
		EKey::UNKNOWN, //VK_OEM_3
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved, 195
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved, 200
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved, 205
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved, 210
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //Reserved, 215
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //VK_OEM_4
		EKey::UNKNOWN, //VK_OEM_5, 220
		EKey::UNKNOWN, //VK_OEM_6
		EKey::UNKNOWN, //VK_OEM_7
		EKey::UNKNOWN, //VK_OEM_8
		EKey::UNKNOWN, //Reserved
		EKey::UNKNOWN, //OEM specific, 225
		EKey::UNKNOWN, //VK_OEM_102
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //VK_PROCESSKEY
		EKey::UNKNOWN, //OEM specific, 230
		EKey::UNKNOWN, //VK_PACKET
		EKey::UNKNOWN, //Unassigned
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific, 235
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific, 240
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific
		EKey::UNKNOWN, //OEM specific, 245
		EKey::UNKNOWN, //VK_ATTN
		EKey::UNKNOWN, //VK_CRSEL
		EKey::UNKNOWN, //VK_EXSEL
		EKey::UNKNOWN, //VK_EREOF
		EKey::UNKNOWN, //VK_PLAY, 250
		EKey::UNKNOWN, //VK_ZOOM
		EKey::UNKNOWN, //VK_NONAME
		EKey::UNKNOWN, //VK_PA1
		EKey::UNKNOWN, //VK_OEM_CLEAR
		EKey::UNKNOWN
	};

	class KeyEventUnicodeResolver{
		std::uint32_t c;
	public:
		KeyEventUnicodeResolver(std::uint32_t unicodeChar = 0) :
			c(unicodeChar)
		{}

		std::vector<std::uint32_t> Resolve()const{
			if (this->c == 0){
				return std::vector<std::uint32_t>();
			}

			std::vector<std::uint32_t> ret(1);
			ret[0] = this->c;
			return std::move(ret);
		}
	};



}//~namespace



namespace morda{

	bool HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& lres){
		morda::App& app = morda::App::inst();

		switch (msg){
		case WM_ACTIVATE:
			if (!HIWORD(wParam)){ //Check Minimization State
				//window active
			}
			else{
				//window is no longer active
			}
			lres = 0;
			return true;

		case WM_SYSCOMMAND:
			switch (wParam){
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
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				0
				);
			lres = 0;
			return true;

		case WM_LBUTTONDOWN:
			app.HandleMouseButton(
				true,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::EMouseButton::LEFT,
				0
				);
			lres = 0;
			return true;

		case WM_LBUTTONUP:
			app.HandleMouseButton(
				false,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::EMouseButton::LEFT,
				0
				);
			lres = 0;
			return true;

		case WM_MBUTTONDOWN:
			app.HandleMouseButton(
				true,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::EMouseButton::MIDDLE,
				0
				);
			lres = 0;
			return true;

		case WM_MBUTTONUP:
			app.HandleMouseButton(
				false,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::EMouseButton::MIDDLE,
				0
				);
			lres = 0;
			return true;

		case WM_RBUTTONDOWN:
			app.HandleMouseButton(
				true,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::EMouseButton::RIGHT,
				0
				);
			lres = 0;
			return true;

		case WM_RBUTTONUP:
			app.HandleMouseButton(
				false,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::EMouseButton::RIGHT,
				0
				);
			lres = 0;
			return true;

		case WM_MOUSEWHEEL:
			//TODO:
			lres = 0;
			return true;

		case WM_KEYDOWN:
		{
			morda::EKey key = keyCodeMap[std::uint8_t(wParam)];
			if ((lParam & 0x40000000) == 0){//ignore auto-repeated keypress event
				app.HandleKeyEvent(true, key);
			}
			app.HandleCharacterInput(KeyEventUnicodeResolver(), key);
			lres = 0;
			return true;
		}
		case WM_KEYUP:
			app.HandleKeyEvent(false, keyCodeMap[std::uint8_t(wParam)]);
			lres = 0;
			return true;

		case WM_CHAR:
			app.HandleCharacterInput(KeyEventUnicodeResolver(std::uint32_t(wParam)), EKey::UNKNOWN);
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
			app.UpdateWindowRect(morda::Rectr(0, 0, float(LOWORD(lParam)), float(HIWORD(lParam))));
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
		if (morda::HandleWindowMessage(hwnd, msg, wParam, lParam, res)){
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
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;//no extra window data
	wc.cbWndExtra = 0;//no extra window data
	wc.hInstance = GetModuleHandle(NULL);// instance handle
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;//no background required for OpenGL
	wc.lpszMenuName = NULL;//we don't want a menu
	wc.lpszClassName = this->name.c_str();// Set the window class Name

	if (!RegisterClass(&wc)){
		throw morda::Exc("Failed to register window class");
	}
}



App::WindowClassWrapper::~WindowClassWrapper()noexcept{
	if (!UnregisterClass(this->name.c_str(), GetModuleHandle(NULL))){
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

	if (!this->hwnd){
		throw morda::Exc("Failed to create a window");
	}

	//NOTE: window will be shown later, right before entering main loop and after all initial App data is initialized

	//	TRACE_AND_LOG(<< "App::WindowWrapper::WindowWrapper(): exit" << std::endl)
}



App::WindowWrapper::~WindowWrapper()noexcept{
	if (!DestroyWindow(this->hwnd)){
		ASSERT_INFO(false, "Failed to destroy window")
	}
}



App::DeviceContextWrapper::DeviceContextWrapper(const WindowParams& wp, const WindowWrapper& w) :
		w(w)
{
	//	TRACE_AND_LOG(<< "App::DeviceContextWrapper::DeviceContextWrapper(): enter" << std::endl)

	this->hdc = GetDC(this->w.hwnd);
	if (!this->hdc){
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

	int pixelFormat = ChoosePixelFormat(this->hdc, &pfd);
	if (!pixelFormat){
		this->Destroy();
		throw morda::Exc("Could not find suitable pixel format");
	}

	//	TRACE_AND_LOG(<< "App::DeviceContextWrapper::DeviceContextWrapper(): pixel format chosen" << std::endl)

	if (!SetPixelFormat(this->hdc, pixelFormat, &pfd)){
		this->Destroy();
		throw morda::Exc("Could not sent pixel format");
	}

	//	TRACE_AND_LOG(<< "App::DeviceContextWrapper::DeviceContextWrapper(): pixel format set" << std::endl)
}



App::DotsPerCmWrapper::DotsPerCmWrapper(DeviceContextWrapper& dc){
	this->value = (float(GetDeviceCaps(dc.hdc, HORZRES)) * 10.0f / float(GetDeviceCaps(dc.hdc, HORZSIZE))
		+ float(GetDeviceCaps(dc.hdc, VERTRES)) * 10.0f / float(GetDeviceCaps(dc.hdc, VERTSIZE))) / 2.0f;
}



void App::DeviceContextWrapper::Destroy()noexcept{
	if (!ReleaseDC(this->w.hwnd, this->hdc)){
		ASSERT_INFO(false, "Failed to release device context")
	}
}




App::ResMan::ResMan(){
	std::string path =
#ifdef DEBUG
		"../../res/"
#else
		"morda_res/"
#endif
		;

	try{
		papki::FSFile fi(path);
		this->MountResPack(fi);
	}
	catch (papki::Exc&){
		//default res pack not found, do nothing
	}
}


App::App(const WindowParams& requestedWindowParams) :
		window(requestedWindowParams, windowClass),
		deviceContext(requestedWindowParams, window),
		dotsPerCm_var(deviceContext),
		curWinRect(0, 0, -1, -1)
{
	this->updateWindowRect(
			morda::Rectr(
					0,
					0,
					float(requestedWindowParams.dim.x),
					float(requestedWindowParams.dim.y)
				)
		);
}



void App::postToUiThread_ts(std::function<void()>&& msg){
	if (PostMessage(this->window.hwnd, WM_USER, 0, reinterpret_cast<LPARAM>(new std::remove_reference<decltype(msg)>::type(std::move(msg)))) == 0){
		throw morda::Exc("PostMessage(): failed");
	}
}



void App::Exec(){
	//	TRACE(<< "App::Exec(): enter" << std::endl)

	while (!this->quitFlag){
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

		if (status == WAIT_OBJECT_0){
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
				//				TRACE(<< "msg got, msg.message = " << msg.message << std::endl)
				if (msg.message == WM_QUIT){
					this->quitFlag = true;
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		this->render();
		//		TRACE(<< "loop" << std::endl)
	}
}

void App::quit()noexcept {
	this->quitFlag = true;
}

namespace morda{

void Main(int argc, const char** argv){
	typedef std::unique_ptr<morda::App>(*Factory)(int, const char**, const utki::Buf<std::uint8_t>&);

	Factory f;
	
	//Try GCC name mangling first
	f = reinterpret_cast<Factory>(GetProcAddress(GetModuleHandle(NULL), TEXT("_ZN5morda9CreateAppEiPPKcN4utki3BufIhEE")));

	if(!f){ //try MSVC function mangling style
		f = reinterpret_cast<Factory>(GetProcAddress(
				GetModuleHandle(NULL),			
#if M_CPU == M_CPU_X86_64
				TEXT("?CreateApp@morda@@YA?AV?$unique_ptr@VApp@morda@@U?$default_delete@VApp@morda@@@std@@@std@@HPEAPEBDV?$Buf@E@utki@@@Z")
#else
				TEXT("?CreateApp@morda@@YA?AV?$unique_ptr@VApp@morda@@U?$default_delete@VApp@morda@@@std@@@std@@HPAPBDV?$Buf@E@utki@@@Z")
#endif
			));
	}

	ASSERT_INFO(f, "no app factory function found")
	std::unique_ptr<morda::App> a = f(argc, argv, nullptr);
	
	ShowWindow(a->window.hwnd, SW_SHOW);
	
	a->Exec();
}

}//~namespace



int WINAPI WinMain(
		HINSTANCE hInstance, // Instance
		HINSTANCE hPrevInstance, // Previous Instance
		LPSTR lpCmdLine, // Command Line Parameters
		int nCmdShow // Window Show State
	)
{
	morda::Main(0, 0);
	
	return 0;
}

