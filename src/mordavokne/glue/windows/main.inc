//This file contains implementations of platform dependent methods from App class.

#include "../../App.hpp"
#include "../../util/util.hpp"

#include <utki/windows.hpp>
#include <papki/FSFile.hpp>

#include <windowsx.h>


using namespace morda;


namespace{

	const std::array<morda::Key_e, std::uint8_t(-1) + 1> keyCodeMap = {
		Key_e::UNKNOWN, //Undefined
		Key_e::UNKNOWN, //VK_LBUTTON
		Key_e::UNKNOWN, //VK_RBUTTON
		Key_e::UNKNOWN, //VK_CANCEL
		Key_e::UNKNOWN, //VK_MBUTTON
		Key_e::UNKNOWN, //VK_XBUTTON1, 5
		Key_e::UNKNOWN, //VK_XBUTTON2
		Key_e::UNKNOWN, //Undefined
		Key_e::BACKSPACE, //VK_BACK = backspace key
		Key_e::TAB, //VK_TAB
		Key_e::UNKNOWN, //Reserved, 10
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //VK_CLEAR = clear key???
		Key_e::ENTER, //VK_RETURN
		Key_e::UNKNOWN, //Undefined
		Key_e::UNKNOWN, //Undefined, 15
		Key_e::LEFT_SHIFT, //VK_SHIFT
		Key_e::LEFT_CONTROL, //VK_CONTROL
		Key_e::LEFT_ALT, //VK_MENU = alt key
		Key_e::PAUSE, //VK_PAUSE
		Key_e::CAPSLOCK, //VK_CAPITAL = caps lock key, 20
		Key_e::UNKNOWN, //VK_KANA, VK_HANGUEL, VK_HANGUL
		Key_e::UNKNOWN, //Undefined
		Key_e::UNKNOWN, //VK_JUNJA
		Key_e::UNKNOWN, //VK_FINAL
		Key_e::UNKNOWN, //VK_HANJA, VK_KANJI, 25
		Key_e::UNKNOWN, //Undefined
		Key_e::ESCAPE, //VK_ESCAPE
		Key_e::UNKNOWN, //VK_CONVERT
		Key_e::UNKNOWN, //VK_NONCONVERT
		Key_e::UNKNOWN, //VK_ACCEPT, 30
		Key_e::UNKNOWN, //VK_MODECHANGE
		Key_e::SPACE, //VK_SPACE = space bar key
		Key_e::PAGE_UP, //VK_PRIOR = page up key
		Key_e::PAGE_DOWN,//VK_NEXT = page down key
		Key_e::END, //VK_END, 35
		Key_e::HOME,//VK_HOME
		Key_e::LEFT,//VK_LEFT
		Key_e::UP, //VK_UP
		Key_e::RIGHT, //VK_RIGHT
		Key_e::DOWN, //VK_DOWN, 40
		Key_e::UNKNOWN, //VK_SELECT
		Key_e::UNKNOWN, //VK_PRINT
		Key_e::UNKNOWN, //VK_EXECUTE
		Key_e::PRINT_SCREEN,//VK_SNAPSHOT = print screen key
		Key_e::INSERT, //VK_INSERT, 45
		Key_e::DELETE, //VK_DELETE
		Key_e::UNKNOWN, //VK_HELP
		Key_e::ZERO, //0 key
		Key_e::ONE, //1 key
		Key_e::TWO, //2 key, 50
		Key_e::THREE, //3 key
		Key_e::FOUR, //4 key
		Key_e::FIVE, //5 key
		Key_e::SIX, //6 key
		Key_e::SEVEN, //7 key, 55
		Key_e::EIGHT, //8 key
		Key_e::NINE, //9 key
		Key_e::UNKNOWN, //Undefined
		Key_e::UNKNOWN, //Undefined
		Key_e::UNKNOWN, //Undefined, 60
		Key_e::UNKNOWN, //Undefined
		Key_e::UNKNOWN, //Undefined
		Key_e::UNKNOWN, //Undefined
		Key_e::UNKNOWN, //Undefined
		Key_e::A, //A key, 65
		Key_e::B, //B key
		Key_e::C, //C key
		Key_e::D, //D key
		Key_e::E, //E key
		Key_e::F, //F key, 70
		Key_e::G, //G key
		Key_e::H, //H key
		Key_e::I, //I key
		Key_e::J, //J key
		Key_e::K, //K key, 75
		Key_e::L, //L key
		Key_e::M, //M key
		Key_e::N, //N key
		Key_e::O, //O key
		Key_e::P, //P key, 80
		Key_e::Q, //Q key
		Key_e::R, //R key
		Key_e::S, //S key
		Key_e::T, //T key
		Key_e::U, //U key, 85
		Key_e::V, //V key
		Key_e::W, //W key
		Key_e::X, //X key
		Key_e::Y, //Y key
		Key_e::Z, //Z key, 90
		Key_e::WINDOWS, //VK_LWIN = left windows key
		Key_e::WINDOWS, //VK_RWIN = right windows key
		Key_e::UNKNOWN, //VK_APPS = applications key
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //VK_SLEEP = computer sleep key, 95
		Key_e::ZERO, //VK_NUMPAD0 = numeric keypad 0 key
		Key_e::ONE, //VK_NUMPAD1 = numeric keypad 1 key
		Key_e::TWO, //VK_NUMPAD2 = numeric keypad 2 key
		Key_e::THREE, //VK_NUMPAD3 = numeric keypad 3 key
		Key_e::FOUR, //VK_NUMPAD4 = numeric keypad 4 key, 100
		Key_e::FIVE, //VK_NUMPAD5 = numeric keypad 5 key
		Key_e::SIX, //VK_NUMPAD6 = numeric keypad 6 key
		Key_e::SEVEN, //VK_NUMPAD7 = numeric keypad 7 key
		Key_e::EIGHT, //VK_NUMPAD8 = numeric keypad 8 key
		Key_e::NINE, //VK_NUMPAD9 = numeric keypad 9 key, 105
		Key_e::UNKNOWN, //VK_MULTIPLY = multiply key
		Key_e::UNKNOWN, //VK_ADD
		Key_e::UNKNOWN, //VK_SEPARATOR
		Key_e::UNKNOWN, //VK_SUBTRACT
		Key_e::UNKNOWN, //VK_DECIMAL, 110
		Key_e::UNKNOWN, //VK_DIVIDE
		Key_e::F1, //VK_F1
		Key_e::F2, //VK_F2
		Key_e::F3, //VK_F3
		Key_e::F4, //VK_F4, 115
		Key_e::F5, //VK_F5
		Key_e::F6, //VK_F6
		Key_e::F7, //VK_F7
		Key_e::F8, //VK_F8
		Key_e::F9, //VK_F9, 120
		Key_e::F10, //VK_F10
		Key_e::F11, //VK_F11
		Key_e::F12, //VK_F12
		Key_e::UNKNOWN, //VK_F13
		Key_e::UNKNOWN, //VK_F14, 125
		Key_e::UNKNOWN, //VK_F15
		Key_e::UNKNOWN, //VK_F16
		Key_e::UNKNOWN, //VK_F17
		Key_e::UNKNOWN, //VK_F18
		Key_e::UNKNOWN, //VK_F19, 130
		Key_e::UNKNOWN, //VK_F20
		Key_e::UNKNOWN, //VK_F21
		Key_e::UNKNOWN, //VK_F22
		Key_e::UNKNOWN, //VK_F23
		Key_e::UNKNOWN, //VK_F24, 135
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned, 140
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //VK_NUMLOCK
		Key_e::UNKNOWN, //VK_SCROLL = scroll lock key, 145
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific, 150
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned, 155
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::LEFT_SHIFT, //VK_LSHIFT, 160
		Key_e::RIGHT_SHIFT, //VK_RSHIFT
		Key_e::LEFT_CONTROL, //VK_LCONTROL
		Key_e::RIGHT_CONTROL, //VK_RCONTROL
		Key_e::WINDOWS_MENU, //VK_LMENU = left menu key
		Key_e::WINDOWS_MENU, //VK_RMENU, 165
		Key_e::UNKNOWN, //VK_BROWSER_BACK
		Key_e::UNKNOWN, //VK_BROWSER_FORWARD
		Key_e::UNKNOWN, //VK_BROWSER_REFRESH
		Key_e::UNKNOWN, //VK_BROWSER_STOP
		Key_e::UNKNOWN, //VK_BROWSER_SEARCH, 170
		Key_e::UNKNOWN, //VK_BROWSER_FAVORITES
		Key_e::UNKNOWN, //VK_BROWSER_HOME
		Key_e::UNKNOWN, //VK_VOLUME_MUTE
		Key_e::UNKNOWN, //VK_VOLUME_DOWN
		Key_e::UNKNOWN, //VK_VOLUME_UP, 175
		Key_e::UNKNOWN, //VK_MEDIA_NEXT_TRACK
		Key_e::UNKNOWN, //VK_MEDIA_PREV_TRACK
		Key_e::UNKNOWN, //VK_MEDIA_STOP
		Key_e::UNKNOWN, //VK_MEDIA_PLAY_PAUSE
		Key_e::UNKNOWN, //VK_LAUNCH_MAIL, 180
		Key_e::UNKNOWN, //VK_LAUNCH_MEDIA_SELECT
		Key_e::UNKNOWN, //VK_LAUNCH_APP1
		Key_e::UNKNOWN, //VK_LAUNCH_APP2
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved, 185
		Key_e::UNKNOWN, //VK_OEM_1
		Key_e::UNKNOWN, //VK_OEM_PLUS
		Key_e::UNKNOWN, //VK_OEM_COMMA
		Key_e::UNKNOWN, //VK_OEM_MINUS
		Key_e::UNKNOWN, //VK_OEM_PERIOD, 190
		Key_e::UNKNOWN, //VK_OEM_2
		Key_e::UNKNOWN, //VK_OEM_3
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved, 195
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved, 200
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved, 205
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved, 210
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //Reserved, 215
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //VK_OEM_4
		Key_e::UNKNOWN, //VK_OEM_5, 220
		Key_e::UNKNOWN, //VK_OEM_6
		Key_e::UNKNOWN, //VK_OEM_7
		Key_e::UNKNOWN, //VK_OEM_8
		Key_e::UNKNOWN, //Reserved
		Key_e::UNKNOWN, //OEM specific, 225
		Key_e::UNKNOWN, //VK_OEM_102
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //VK_PROCESSKEY
		Key_e::UNKNOWN, //OEM specific, 230
		Key_e::UNKNOWN, //VK_PACKET
		Key_e::UNKNOWN, //Unassigned
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific, 235
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific, 240
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific
		Key_e::UNKNOWN, //OEM specific, 245
		Key_e::UNKNOWN, //VK_ATTN
		Key_e::UNKNOWN, //VK_CRSEL
		Key_e::UNKNOWN, //VK_EXSEL
		Key_e::UNKNOWN, //VK_EREOF
		Key_e::UNKNOWN, //VK_PLAY, 250
		Key_e::UNKNOWN, //VK_ZOOM
		Key_e::UNKNOWN, //VK_NONAME
		Key_e::UNKNOWN, //VK_PA1
		Key_e::UNKNOWN, //VK_OEM_CLEAR
		Key_e::UNKNOWN
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
			if(!app.window.isHovered){
				
				TRACKMOUSEEVENT tme = { sizeof(tme) };
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				TrackMouseEvent(&tme);

				app.window.isHovered = true;

				//restore mouse cursor invisibility
				if(!app.mouseCursorIsCurrentlyVisible){
					CURSORINFO ci;
					ci.cbSize = sizeof(CURSORINFO);
					if(GetCursorInfo(&ci) != 0){
						if(ci.flags & CURSOR_SHOWING){
							ShowCursor(FALSE);
						}
					}else{
						TRACE(<< "GetCursorInfo(): failed!!!" << std::endl)
					}
				}

				app.handleMouseHover(true, 0);
			}
			app.handleMouseMove(
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				0
				);
			lres = 0;
			return true;
		case WM_MOUSELEAVE:
			//Windows hides the mouse cursor even in non-client areas of the window,
			//like caption bar and borders, so show cursor if it is hidden
			if(!app.mouseCursorIsCurrentlyVisible){
				ShowCursor(TRUE);
			}

			app.window.isHovered = false;
			app.handleMouseHover(false, 0);

			//Report mouse button up events for all pressed mouse buttons
			for(
					decltype(app.window.mouseButtonState)::index_t i = 0;
					i != app.window.mouseButtonState.size();
					++i
				)
			{
				if(app.window.mouseButtonState.get(i)){
					app.window.mouseButtonState.clear(i);
					app.handleMouseButton(
							false,
							morda::Vec2r(1000000, 1000000), //outside of the window
							Widget::MouseButton_e(i),
							0
						);
				}
			}

			lres = 0;
			return true;
		case WM_LBUTTONDOWN:
			app.window.mouseButtonState.set(Widget::MouseButton_e::LEFT);
			app.handleMouseButton(
				true,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::MouseButton_e::LEFT,
				0
				);
			lres = 0;
			return true;

		case WM_LBUTTONUP:
			app.window.mouseButtonState.clear(Widget::MouseButton_e::LEFT);
			app.handleMouseButton(
				false,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::MouseButton_e::LEFT,
				0
				);
			lres = 0;
			return true;

		case WM_MBUTTONDOWN:
			app.window.mouseButtonState.set(Widget::MouseButton_e::MIDDLE);
			app.handleMouseButton(
				true,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::MouseButton_e::MIDDLE,
				0
				);
			lres = 0;
			return true;

		case WM_MBUTTONUP:
			app.window.mouseButtonState.clear(Widget::MouseButton_e::MIDDLE);
			app.handleMouseButton(
				false,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::MouseButton_e::MIDDLE,
				0
				);
			lres = 0;
			return true;

		case WM_RBUTTONDOWN:
			app.window.mouseButtonState.set(Widget::MouseButton_e::RIGHT);
			app.handleMouseButton(
				true,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::MouseButton_e::RIGHT,
				0
				);
			lres = 0;
			return true;

		case WM_RBUTTONUP:
			app.window.mouseButtonState.clear(Widget::MouseButton_e::RIGHT);
			app.handleMouseButton(
				false,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				Widget::MouseButton_e::RIGHT,
				0
				);
			lres = 0;
			return true;

		case WM_MOUSEWHEEL:
			{
				short int times = HIWORD(wParam);
				times /= 120;
				Widget::MouseButton_e button;
				if(times >= 0){
					button = Widget::MouseButton_e::WHEEL_UP;
				}else{
					times = -times;
					button = Widget::MouseButton_e::WHEEL_DOWN;
				}
				
				POINT pos;
				pos.x = GET_X_LPARAM(lParam);
				pos.y = GET_Y_LPARAM(lParam);

				//For some reason in WM_MOUSEWHEEL message mouse cursor position is sent in screen coordinates,
				//need to traslate those to window coordinates.
				if(ScreenToClient(hwnd, &pos) == 0){
					break;
				}

				for(unsigned i = 0; i != times; ++i){
					app.handleMouseButton(
							true,
							morda::Vec2r(float(pos.x), float(pos.y)),
							button,
							0
						);
					app.handleMouseButton(
							false,
							morda::Vec2r(float(pos.x), float(pos.y)),
							button,
							0
						);
				}
			}
			lres = 0;
			return true;

		case WM_KEYDOWN:
		{
			morda::Key_e key = keyCodeMap[std::uint8_t(wParam)];
			if ((lParam & 0x40000000) == 0){//ignore auto-repeated keypress event
				app.handleKeyEvent(true, key);
			}
			app.handleCharacterInput(KeyEventUnicodeResolver(), key);
			lres = 0;
			return true;
		}
		case WM_KEYUP:
			app.handleKeyEvent(false, keyCodeMap[std::uint8_t(wParam)]);
			lres = 0;
			return true;

		case WM_CHAR:
			app.handleCharacterInput(KeyEventUnicodeResolver(std::uint32_t(wParam)), Key_e::UNKNOWN);
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
			app.updateWindowRect(morda::Rectr(0, 0, float(LOWORD(lParam)), float(HIWORD(lParam))));
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
	
	static	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1, // Version number of the structure, should be 1
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		BYTE(PFD_TYPE_RGBA),
		BYTE(32), //color depth
		BYTE(0), BYTE(0), BYTE(0), BYTE(0), BYTE(0), BYTE(0), //color bits ignored
		BYTE(0), //no alpha buffer
		BYTE(0), //shift bit ignored
		BYTE(0), //no accumulation buffer
		BYTE(0), BYTE(0), BYTE(0), BYTE(0), //accumulation bits ignored
		wp.buffers.get(WindowParams::Buffer_e::DEPTH) ? BYTE(16) : BYTE(0), //16bit depth buffer
		wp.buffers.get(WindowParams::Buffer_e::STENCIL) ? BYTE(8) : BYTE(0),
		BYTE(0), //no auxiliary buffer
		BYTE(PFD_MAIN_PLANE), //main drawing layer
		BYTE(0), //reserved
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



void App::DeviceContextWrapper::Destroy()noexcept{
	if (!ReleaseDC(this->w.hwnd, this->hdc)){
		ASSERT_INFO(false, "Failed to release device context")
	}
}




namespace{

real getDotsPerInch(HDC dc){
	real value = (real(GetDeviceCaps(dc, HORZRES)) * 10.0f / real(GetDeviceCaps(dc, HORZSIZE))
		+ real(GetDeviceCaps(dc, VERTRES)) * 10.0f / real(GetDeviceCaps(dc, VERTSIZE))) / 2.0f;
	value *= 2.54f;
	return value;
}

real getDotsPerPt(HDC dc){
	kolme::Vec2ui resolution(GetDeviceCaps(dc, HORZRES), GetDeviceCaps(dc, VERTRES));
	kolme::Vec2ui screenSizeMm(GetDeviceCaps(dc, HORZSIZE), GetDeviceCaps(dc, VERTSIZE));

	return findDotsPerPt(resolution, screenSizeMm);
}

}//~namespace


App::App(const WindowParams& requestedWindowParams) :
		windowParams(requestedWindowParams),
		window(requestedWindowParams, windowClass),
		deviceContext(requestedWindowParams, window),
		curWinRect(0, 0, -1, -1),
		units(getDotsPerInch(deviceContext.hdc), getDotsPerPt(deviceContext.hdc))
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
		std::uint32_t timeout = this->updater.update();
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
				TEXT("?createApp@morda@@YA?AV?$unique_ptr@VApp@morda@@U?$default_delete@VApp@morda@@@std@@@std@@HPEAPEBDV?$Buf@E@utki@@@Z")
#else
				TEXT("?createApp@morda@@YA?AV?$unique_ptr@VApp@morda@@U?$default_delete@VApp@morda@@@std@@@std@@HPAPBDV?$Buf@E@utki@@@Z")
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


void App::setFullscreen(bool enable) {
	if (enable == this->isFullscreen()) {
		return;
	}

	if (enable) {
		//save original window size
		RECT rect;
		if (GetWindowRect(this->window.hwnd, &rect) == 0) {
			throw Exc("Failed to get window rect");
		}
		this->beforeFullScreenWindowRect.p.x = rect.left;
		this->beforeFullScreenWindowRect.p.y = rect.top;
		this->beforeFullScreenWindowRect.d.x = rect.right - rect.left;
		this->beforeFullScreenWindowRect.d.y = rect.bottom - rect.top;

		// Set new window style
		SetWindowLong(
				this->window.hwnd,
				GWL_STYLE,
				GetWindowLong(this->window.hwnd, GWL_STYLE)
						& ~(WS_CAPTION | WS_THICKFRAME)
			);
		SetWindowLong(
				this->window.hwnd,
				GWL_EXSTYLE,
				GetWindowLong(this->window.hwnd, GWL_EXSTYLE)
						& ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)
			);

		//set new window size and position
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(MonitorFromWindow(this->window.hwnd, MONITOR_DEFAULTTONEAREST), &mi);
		SetWindowPos(
				this->window.hwnd,
				NULL,
				mi.rcMonitor.left,
				mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
			);
	}else{
		// Reset original window style
		SetWindowLong(
				this->window.hwnd,
				GWL_STYLE,
				GetWindowLong(this->window.hwnd, GWL_STYLE)
						| (WS_CAPTION | WS_THICKFRAME)
			);
		SetWindowLong(
				this->window.hwnd,
				GWL_EXSTYLE,
				GetWindowLong(this->window.hwnd, GWL_EXSTYLE)
						| (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)
			);

			SetWindowPos(
					this->window.hwnd,
					NULL,
					this->beforeFullScreenWindowRect.p.x,
					this->beforeFullScreenWindowRect.p.y,
					this->beforeFullScreenWindowRect.d.x,
					this->beforeFullScreenWindowRect.d.y,
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
				);
	}

	this->isFullscreen_v = enable;
}


void App::setMouseCursorVisible(bool visible){
	if(visible){
		if(!this->mouseCursorIsCurrentlyVisible){
			ShowCursor(TRUE);
			this->mouseCursorIsCurrentlyVisible = true;
		}
	}else{
		if(this->mouseCursorIsCurrentlyVisible){
			ShowCursor(FALSE);
			this->mouseCursorIsCurrentlyVisible = false;
		}
	}
}
