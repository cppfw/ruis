//This file contains implementations of platform dependent methods from App class.

#include "../../App.hpp"
#include "../../../../../../src/morda/util/util.hpp"

#include "../../../../../../src/morda/Morda.hpp"

#include <utki/windows.hpp>
#include <papki/FSFile.hpp>

#include <windowsx.h>


using namespace mordavokne;


namespace{

	const std::array<morda::Key_e, std::uint8_t(-1) + 1> keyCodeMap = {
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::UNKNOWN, //VK_LBUTTON
		morda::Key_e::UNKNOWN, //VK_RBUTTON
		morda::Key_e::UNKNOWN, //VK_CANCEL
		morda::Key_e::UNKNOWN, //VK_MBUTTON
		morda::Key_e::UNKNOWN, //VK_XBUTTON1, 5
		morda::Key_e::UNKNOWN, //VK_XBUTTON2
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::BACKSPACE, //VK_BACK = backspace key
		morda::Key_e::TAB, //VK_TAB
		morda::Key_e::UNKNOWN, //Reserved, 10
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //VK_CLEAR = clear key???
		morda::Key_e::ENTER, //VK_RETURN
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::UNKNOWN, //Undefined, 15
		morda::Key_e::LEFT_SHIFT, //VK_SHIFT
		morda::Key_e::LEFT_CONTROL, //VK_CONTROL
		morda::Key_e::LEFT_ALT, //VK_MENU = alt key
		morda::Key_e::PAUSE, //VK_PAUSE
		morda::Key_e::CAPSLOCK, //VK_CAPITAL = caps lock key, 20
		morda::Key_e::UNKNOWN, //VK_KANA, VK_HANGUEL, VK_HANGUL
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::UNKNOWN, //VK_JUNJA
		morda::Key_e::UNKNOWN, //VK_FINAL
		morda::Key_e::UNKNOWN, //VK_HANJA, VK_KANJI, 25
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::ESCAPE, //VK_ESCAPE
		morda::Key_e::UNKNOWN, //VK_CONVERT
		morda::Key_e::UNKNOWN, //VK_NONCONVERT
		morda::Key_e::UNKNOWN, //VK_ACCEPT, 30
		morda::Key_e::UNKNOWN, //VK_MODECHANGE
		morda::Key_e::SPACE, //VK_SPACE = space bar key
		morda::Key_e::PAGE_UP, //VK_PRIOR = page up key
		morda::Key_e::PAGE_DOWN,//VK_NEXT = page down key
		morda::Key_e::END, //VK_END, 35
		morda::Key_e::HOME,//VK_HOME
		morda::Key_e::LEFT,//VK_LEFT
		morda::Key_e::UP, //VK_UP
		morda::Key_e::RIGHT, //VK_RIGHT
		morda::Key_e::DOWN, //VK_DOWN, 40
		morda::Key_e::UNKNOWN, //VK_SELECT
		morda::Key_e::UNKNOWN, //VK_PRINT
		morda::Key_e::UNKNOWN, //VK_EXECUTE
		morda::Key_e::PRINT_SCREEN,//VK_SNAPSHOT = print screen key
		morda::Key_e::INSERT, //VK_INSERT, 45
		morda::Key_e::DELETE, //VK_DELETE
		morda::Key_e::UNKNOWN, //VK_HELP
		morda::Key_e::ZERO, //0 key
		morda::Key_e::ONE, //1 key
		morda::Key_e::TWO, //2 key, 50
		morda::Key_e::THREE, //3 key
		morda::Key_e::FOUR, //4 key
		morda::Key_e::FIVE, //5 key
		morda::Key_e::SIX, //6 key
		morda::Key_e::SEVEN, //7 key, 55
		morda::Key_e::EIGHT, //8 key
		morda::Key_e::NINE, //9 key
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::UNKNOWN, //Undefined, 60
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::UNKNOWN, //Undefined
		morda::Key_e::A, //A key, 65
		morda::Key_e::B, //B key
		morda::Key_e::C, //C key
		morda::Key_e::D, //D key
		morda::Key_e::E, //E key
		morda::Key_e::F, //F key, 70
		morda::Key_e::G, //G key
		morda::Key_e::H, //H key
		morda::Key_e::I, //I key
		morda::Key_e::J, //J key
		morda::Key_e::K, //K key, 75
		morda::Key_e::L, //L key
		morda::Key_e::M, //M key
		morda::Key_e::N, //N key
		morda::Key_e::O, //O key
		morda::Key_e::P, //P key, 80
		morda::Key_e::Q, //Q key
		morda::Key_e::R, //R key
		morda::Key_e::S, //S key
		morda::Key_e::T, //T key
		morda::Key_e::U, //U key, 85
		morda::Key_e::V, //V key
		morda::Key_e::W, //W key
		morda::Key_e::X, //X key
		morda::Key_e::Y, //Y key
		morda::Key_e::Z, //Z key, 90
		morda::Key_e::WINDOWS, //VK_LWIN = left windows key
		morda::Key_e::WINDOWS, //VK_RWIN = right windows key
		morda::Key_e::UNKNOWN, //VK_APPS = applications key
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //VK_SLEEP = computer sleep key, 95
		morda::Key_e::ZERO, //VK_NUMPAD0 = numeric keypad 0 key
		morda::Key_e::ONE, //VK_NUMPAD1 = numeric keypad 1 key
		morda::Key_e::TWO, //VK_NUMPAD2 = numeric keypad 2 key
		morda::Key_e::THREE, //VK_NUMPAD3 = numeric keypad 3 key
		morda::Key_e::FOUR, //VK_NUMPAD4 = numeric keypad 4 key, 100
		morda::Key_e::FIVE, //VK_NUMPAD5 = numeric keypad 5 key
		morda::Key_e::SIX, //VK_NUMPAD6 = numeric keypad 6 key
		morda::Key_e::SEVEN, //VK_NUMPAD7 = numeric keypad 7 key
		morda::Key_e::EIGHT, //VK_NUMPAD8 = numeric keypad 8 key
		morda::Key_e::NINE, //VK_NUMPAD9 = numeric keypad 9 key, 105
		morda::Key_e::UNKNOWN, //VK_MULTIPLY = multiply key
		morda::Key_e::UNKNOWN, //VK_ADD
		morda::Key_e::UNKNOWN, //VK_SEPARATOR
		morda::Key_e::UNKNOWN, //VK_SUBTRACT
		morda::Key_e::UNKNOWN, //VK_DECIMAL, 110
		morda::Key_e::UNKNOWN, //VK_DIVIDE
		morda::Key_e::F1, //VK_F1
		morda::Key_e::F2, //VK_F2
		morda::Key_e::F3, //VK_F3
		morda::Key_e::F4, //VK_F4, 115
		morda::Key_e::F5, //VK_F5
		morda::Key_e::F6, //VK_F6
		morda::Key_e::F7, //VK_F7
		morda::Key_e::F8, //VK_F8
		morda::Key_e::F9, //VK_F9, 120
		morda::Key_e::F10, //VK_F10
		morda::Key_e::F11, //VK_F11
		morda::Key_e::F12, //VK_F12
		morda::Key_e::UNKNOWN, //VK_F13
		morda::Key_e::UNKNOWN, //VK_F14, 125
		morda::Key_e::UNKNOWN, //VK_F15
		morda::Key_e::UNKNOWN, //VK_F16
		morda::Key_e::UNKNOWN, //VK_F17
		morda::Key_e::UNKNOWN, //VK_F18
		morda::Key_e::UNKNOWN, //VK_F19, 130
		morda::Key_e::UNKNOWN, //VK_F20
		morda::Key_e::UNKNOWN, //VK_F21
		morda::Key_e::UNKNOWN, //VK_F22
		morda::Key_e::UNKNOWN, //VK_F23
		morda::Key_e::UNKNOWN, //VK_F24, 135
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned, 140
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //VK_NUMLOCK
		morda::Key_e::UNKNOWN, //VK_SCROLL = scroll lock key, 145
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific, 150
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned, 155
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::LEFT_SHIFT, //VK_LSHIFT, 160
		morda::Key_e::RIGHT_SHIFT, //VK_RSHIFT
		morda::Key_e::LEFT_CONTROL, //VK_LCONTROL
		morda::Key_e::RIGHT_CONTROL, //VK_RCONTROL
		morda::Key_e::WINDOWS_MENU, //VK_LMENU = left menu key
		morda::Key_e::WINDOWS_MENU, //VK_RMENU, 165
		morda::Key_e::UNKNOWN, //VK_BROWSER_BACK
		morda::Key_e::UNKNOWN, //VK_BROWSER_FORWARD
		morda::Key_e::UNKNOWN, //VK_BROWSER_REFRESH
		morda::Key_e::UNKNOWN, //VK_BROWSER_STOP
		morda::Key_e::UNKNOWN, //VK_BROWSER_SEARCH, 170
		morda::Key_e::UNKNOWN, //VK_BROWSER_FAVORITES
		morda::Key_e::UNKNOWN, //VK_BROWSER_HOME
		morda::Key_e::UNKNOWN, //VK_VOLUME_MUTE
		morda::Key_e::UNKNOWN, //VK_VOLUME_DOWN
		morda::Key_e::UNKNOWN, //VK_VOLUME_UP, 175
		morda::Key_e::UNKNOWN, //VK_MEDIA_NEXT_TRACK
		morda::Key_e::UNKNOWN, //VK_MEDIA_PREV_TRACK
		morda::Key_e::UNKNOWN, //VK_MEDIA_STOP
		morda::Key_e::UNKNOWN, //VK_MEDIA_PLAY_PAUSE
		morda::Key_e::UNKNOWN, //VK_LAUNCH_MAIL, 180
		morda::Key_e::UNKNOWN, //VK_LAUNCH_MEDIA_SELECT
		morda::Key_e::UNKNOWN, //VK_LAUNCH_APP1
		morda::Key_e::UNKNOWN, //VK_LAUNCH_APP2
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved, 185
		morda::Key_e::UNKNOWN, //VK_OEM_1
		morda::Key_e::UNKNOWN, //VK_OEM_PLUS
		morda::Key_e::UNKNOWN, //VK_OEM_COMMA
		morda::Key_e::UNKNOWN, //VK_OEM_MINUS
		morda::Key_e::UNKNOWN, //VK_OEM_PERIOD, 190
		morda::Key_e::UNKNOWN, //VK_OEM_2
		morda::Key_e::UNKNOWN, //VK_OEM_3
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved, 195
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved, 200
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved, 205
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved, 210
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //Reserved, 215
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //VK_OEM_4
		morda::Key_e::UNKNOWN, //VK_OEM_5, 220
		morda::Key_e::UNKNOWN, //VK_OEM_6
		morda::Key_e::UNKNOWN, //VK_OEM_7
		morda::Key_e::UNKNOWN, //VK_OEM_8
		morda::Key_e::UNKNOWN, //Reserved
		morda::Key_e::UNKNOWN, //OEM specific, 225
		morda::Key_e::UNKNOWN, //VK_OEM_102
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //VK_PROCESSKEY
		morda::Key_e::UNKNOWN, //OEM specific, 230
		morda::Key_e::UNKNOWN, //VK_PACKET
		morda::Key_e::UNKNOWN, //Unassigned
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific, 235
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific, 240
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific
		morda::Key_e::UNKNOWN, //OEM specific, 245
		morda::Key_e::UNKNOWN, //VK_ATTN
		morda::Key_e::UNKNOWN, //VK_CRSEL
		morda::Key_e::UNKNOWN, //VK_EXSEL
		morda::Key_e::UNKNOWN, //VK_EREOF
		morda::Key_e::UNKNOWN, //VK_PLAY, 250
		morda::Key_e::UNKNOWN, //VK_ZOOM
		morda::Key_e::UNKNOWN, //VK_NONAME
		morda::Key_e::UNKNOWN, //VK_PA1
		morda::Key_e::UNKNOWN, //VK_OEM_CLEAR
		morda::Key_e::UNKNOWN
	};

	class KeyEventUnicodeProvider : public morda::Morda::UnicodeProvider{
		char32_t c;
	public:
		KeyEventUnicodeProvider(std::uint32_t unicodeChar = 0) :
			c(unicodeChar)
		{}

		std::u32string get()const override{
			if (this->c == 0){
				return std::u32string();
			}

			return std::u32string(&this->c, 1);
		}
	};



}//~namespace



namespace mordavokne{

	bool handleWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& lres){
		mordavokne::App& app = mordavokne::App::inst();

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
							morda::MouseButton_e(i),
							0
						);
				}
			}

			lres = 0;
			return true;
		case WM_LBUTTONDOWN:
			app.window.mouseButtonState.set(morda::MouseButton_e::LEFT);
			app.handleMouseButton(
				true,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				morda::MouseButton_e::LEFT,
				0
				);
			lres = 0;
			return true;

		case WM_LBUTTONUP:
			app.window.mouseButtonState.clear(morda::MouseButton_e::LEFT);
			app.handleMouseButton(
				false,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				morda::MouseButton_e::LEFT,
				0
				);
			lres = 0;
			return true;

		case WM_MBUTTONDOWN:
			app.window.mouseButtonState.set(morda::MouseButton_e::MIDDLE);
			app.handleMouseButton(
				true,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				morda::MouseButton_e::MIDDLE,
				0
				);
			lres = 0;
			return true;

		case WM_MBUTTONUP:
			app.window.mouseButtonState.clear(morda::MouseButton_e::MIDDLE);
			app.handleMouseButton(
				false,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				morda::MouseButton_e::MIDDLE,
				0
				);
			lres = 0;
			return true;

		case WM_RBUTTONDOWN:
			app.window.mouseButtonState.set(morda::MouseButton_e::RIGHT);
			app.handleMouseButton(
				true,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				morda::MouseButton_e::RIGHT,
				0
				);
			lres = 0;
			return true;

		case WM_RBUTTONUP:
			app.window.mouseButtonState.clear(morda::MouseButton_e::RIGHT);
			app.handleMouseButton(
				false,
				morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
				morda::MouseButton_e::RIGHT,
				0
				);
			lres = 0;
			return true;

		case WM_MOUSEWHEEL:
			{
				short int times = HIWORD(wParam);
				times /= 120;
				morda::MouseButton_e button;
				if(times >= 0){
					button = morda::MouseButton_e::WHEEL_UP;
				}else{
					times = -times;
					button = morda::MouseButton_e::WHEEL_DOWN;
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
			app.handleCharacterInput(KeyEventUnicodeProvider(), key);
			lres = 0;
			return true;
		}
		case WM_KEYUP:
			app.handleKeyEvent(false, keyCodeMap[std::uint8_t(wParam)]);
			lres = 0;
			return true;

		case WM_CHAR:
			app.handleCharacterInput(KeyEventUnicodeProvider(std::uint32_t(wParam)), morda::Key_e::UNKNOWN);
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
		if (mordavokne::handleWindowMessage(hwnd, msg, wParam, lParam, res)){
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




App::OpenGLContextWrapper::OpenGLContextWrapper(HDC hdc) {
	//	TRACE_AND_LOG(<< "App::GLContextWrapper::GLContextWrapper(): enter" << std::endl)

	this->hrc = wglCreateContext(hdc);
	if (!this->hrc) {
		throw morda::Exc("Failed to create OpenGL rendering context");
	}

	//	TRACE_AND_LOG(<< "App::GLContextWrapper::GLContextWrapper(): GL rendering context created" << std::endl)

	if (!wglMakeCurrent(hdc, this->hrc)) {
		this->Destroy();
		throw morda::Exc("Failed to activate OpenGL rendering context");
	}

	//	TRACE_AND_LOG(<< "App::GLContextWrapper::GLContextWrapper(): GL rendering context created" << std::endl)
}

void App::OpenGLContextWrapper::Destroy()noexcept{
	if (!wglMakeCurrent(NULL, NULL)) {
		ASSERT_INFO(false, "Deactivating OpenGL rendering context failed")
	}

	if (!wglDeleteContext(this->hrc)) {
		ASSERT_INFO(false, "Releasing OpenGL rendering context failed")
	}
}


namespace{

morda::real getDotsPerInch(HDC dc){
	morda::real value = (morda::real(GetDeviceCaps(dc, HORZRES)) * 10.0f / morda::real(GetDeviceCaps(dc, HORZSIZE))
		+ morda::real(GetDeviceCaps(dc, VERTRES)) * 10.0f / morda::real(GetDeviceCaps(dc, VERTSIZE))) / 2.0f;
	value *= 2.54f;
	return value;
}

morda::real getDotsPerPt(HDC dc){
	kolme::Vec2ui resolution(GetDeviceCaps(dc, HORZRES), GetDeviceCaps(dc, VERTRES));
	kolme::Vec2ui screenSizeMm(GetDeviceCaps(dc, HORZSIZE), GetDeviceCaps(dc, VERTSIZE));

	return morda::findDotsPerPt(resolution, screenSizeMm);
}

}//~namespace


App::App(const WindowParams& requestedWindowParams) :
		window(requestedWindowParams, windowClass),
		deviceContext(requestedWindowParams, window),
		openglContext(deviceContext.hdc),
		curWinRect(0, 0, -1, -1),
		gui(getDotsPerInch(deviceContext.hdc), getDotsPerPt(deviceContext.hdc))
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



void App::exec(){
	//	TRACE(<< "App::exec(): enter" << std::endl)

	while (!this->quitFlag){
		std::uint32_t timeout = this->gui.update();
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

namespace mordavokne{

void winmain(int argc, const char** argv){
	typedef std::unique_ptr<mordavokne::App>(*Factory)(int, const char**, const utki::Buf<std::uint8_t>&);

	Factory f;

	//Try GCC name mangling first
	f = reinterpret_cast<Factory>(GetProcAddress(GetModuleHandle(NULL), TEXT("_ZN10mordavokne9createAppEiPPKcN4utki3BufIhEE")));

	if(!f){ //try MSVC function mangling style
		f = reinterpret_cast<Factory>(GetProcAddress(
				GetModuleHandle(NULL),			
#if M_CPU == M_CPU_X86_64
				TEXT("?createApp@mordavokne@@YA?AV?$unique_ptr@VApp@mordavokne@@U?$default_delete@VApp@mordavokne@@@std@@@std@@HPEAPEBDV?$Buf@E@utki@@@Z")
#else
				TEXT("?createApp@mordavokne@@YA?AV?$unique_ptr@VApp@mordavokne@@U?$default_delete@VApp@mordavokne@@@std@@@std@@HPAPBDV?$Buf@E@utki@@@Z")
#endif
			));
	}

	ASSERT_INFO(f, "no app factory function found")
	std::unique_ptr<mordavokne::App> a = f(argc, argv, nullptr);

	ShowWindow(a->window.hwnd, SW_SHOW);

	a->exec();
}

}//~namespace



int WINAPI WinMain(
		HINSTANCE hInstance, // Instance
		HINSTANCE hPrevInstance, // Previous Instance
		LPSTR lpCmdLine, // Command Line Parameters
		int nCmdShow // Window Show State
	)
{
	mordavokne::winmain(0, 0);

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
			throw utki::Exc("Failed to get window rect");
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
