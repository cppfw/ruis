#include <morda/util/util.hpp>
#include <morda/Morda.hpp>

#include <utki/windows.hpp>
#include <utki/Unique.hpp>

#include <papki/FSFile.hpp>

#include <windowsx.h>
#include <Shlobj.h>

#include <mordaren/OpenGL2Renderer.hpp>

#include "../../application.hpp"

#include "../friendAccessors.cxx"

using namespace mordavokne;


namespace{
struct WindowWrapper : public utki::Unique{
	std::string windowClassName;
	HWND hwnd;
	HDC hdc;
	HGLRC hrc;

	bool quitFlag = false;

	bool isHovered = false; // for tracking when mouse enters or leaves window.

	utki::Flags<morda::MouseButton_e> mouseButtonState;

	bool mouseCursorIsCurrentlyVisible = true;

	WindowWrapper(const window_params& wp);

	~WindowWrapper()noexcept;
};

WindowWrapper& getImpl(const std::unique_ptr<utki::Unique>& pimpl){
	ASSERT(dynamic_cast<WindowWrapper*>(pimpl.get()))
	return static_cast<WindowWrapper&>(*pimpl);
}

}

namespace{

const std::array<morda::key, std::uint8_t(-1) + 1> keyCodeMap = {
	morda::key::UNKNOWN, // Undefined
	morda::key::UNKNOWN, // VK_LBUTTON
	morda::key::UNKNOWN, // VK_RBUTTON
	morda::key::UNKNOWN, // VK_CANCEL
	morda::key::UNKNOWN, // VK_MBUTTON
	morda::key::UNKNOWN, // VK_XBUTTON1, 5
	morda::key::UNKNOWN, // VK_XBUTTON2
	morda::key::UNKNOWN, // Undefined
	morda::key::BACKSPACE, // VK_BACK = backspace key
	morda::key::TAB, // VK_TAB
	morda::key::UNKNOWN, // Reserved, 10
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // VK_CLEAR = clear key???
	morda::key::ENTER, // VK_RETURN
	morda::key::UNKNOWN, // Undefined
	morda::key::UNKNOWN, // Undefined, 15
	morda::key::LEFT_SHIFT, // VK_SHIFT
	morda::key::LEFT_CONTROL, // VK_CONTROL
	morda::key::LEFT_ALT, // VK_MENU = alt key
	morda::key::PAUSE, // VK_PAUSE
	morda::key::CAPSLOCK, // VK_CAPITAL = caps lock key, 20
	morda::key::UNKNOWN, // VK_KANA, VK_HANGUEL, VK_HANGUL
	morda::key::UNKNOWN, // Undefined
	morda::key::UNKNOWN, // VK_JUNJA
	morda::key::UNKNOWN, // VK_FINAL
	morda::key::UNKNOWN, // VK_HANJA, VK_KANJI, 25
	morda::key::UNKNOWN, // Undefined
	morda::key::ESCAPE, // VK_ESCAPE
	morda::key::UNKNOWN, // VK_CONVERT
	morda::key::UNKNOWN, // VK_NONCONVERT
	morda::key::UNKNOWN, // VK_ACCEPT, 30
	morda::key::UNKNOWN, // VK_MODECHANGE
	morda::key::SPACE, // VK_SPACE = space bar key
	morda::key::PAGE_UP, // VK_PRIOR = page up key
	morda::key::PAGE_DOWN,// VK_NEXT = page down key
	morda::key::END, // VK_END, 35
	morda::key::HOME,// VK_HOME
	morda::key::LEFT,// VK_LEFT
	morda::key::UP, // VK_UP
	morda::key::RIGHT, // VK_RIGHT
	morda::key::DOWN, // VK_DOWN, 40
	morda::key::UNKNOWN, // VK_SELECT
	morda::key::UNKNOWN, // VK_PRINT
	morda::key::UNKNOWN, // VK_EXECUTE
	morda::key::PRINT_SCREEN,// VK_SNAPSHOT = print screen key
	morda::key::INSERT, // VK_INSERT, 45
	morda::key::DELETE, // VK_DELETE
	morda::key::UNKNOWN, // VK_HELP
	morda::key::ZERO, // 0 key
	morda::key::ONE, // 1 key
	morda::key::TWO, // 2 key, 50
	morda::key::THREE, // 3 key
	morda::key::FOUR, // 4 key
	morda::key::FIVE, // 5 key
	morda::key::SIX, // 6 key
	morda::key::SEVEN, // 7 key, 55
	morda::key::EIGHT, // 8 key
	morda::key::NINE, // 9 key
	morda::key::UNKNOWN, // Undefined
	morda::key::UNKNOWN, // Undefined
	morda::key::UNKNOWN, // Undefined, 60
	morda::key::UNKNOWN, // Undefined
	morda::key::UNKNOWN, // Undefined
	morda::key::UNKNOWN, // Undefined
	morda::key::UNKNOWN, // Undefined
	morda::key::A, // A key, 65
	morda::key::B, // B key
	morda::key::C, // C key
	morda::key::D, // D key
	morda::key::E, // E key
	morda::key::F, // F key, 70
	morda::key::G, // G key
	morda::key::H, // H key
	morda::key::I, // I key
	morda::key::J, // J key
	morda::key::K, // K key, 75
	morda::key::L, // L key
	morda::key::M, // M key
	morda::key::N, // N key
	morda::key::O, // O key
	morda::key::P, // P key, 80
	morda::key::Q, // Q key
	morda::key::R, // R key
	morda::key::S, // S key
	morda::key::T, // T key
	morda::key::U, // U key, 85
	morda::key::V, // V key
	morda::key::W, // W key
	morda::key::X, // X key
	morda::key::Y, // Y key
	morda::key::Z, // Z key, 90
	morda::key::WINDOWS, // VK_LWIN = left windows key
	morda::key::WINDOWS, // VK_RWIN = right windows key
	morda::key::UNKNOWN, // VK_APPS = applications key
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // VK_SLEEP = computer sleep key, 95
	morda::key::ZERO, // VK_NUMPAD0 = numeric keypad 0 key
	morda::key::ONE, // VK_NUMPAD1 = numeric keypad 1 key
	morda::key::TWO, // VK_NUMPAD2 = numeric keypad 2 key
	morda::key::THREE, // VK_NUMPAD3 = numeric keypad 3 key
	morda::key::FOUR, // VK_NUMPAD4 = numeric keypad 4 key, 100
	morda::key::FIVE, // VK_NUMPAD5 = numeric keypad 5 key
	morda::key::SIX, // VK_NUMPAD6 = numeric keypad 6 key
	morda::key::SEVEN, // VK_NUMPAD7 = numeric keypad 7 key
	morda::key::EIGHT, // VK_NUMPAD8 = numeric keypad 8 key
	morda::key::NINE, // VK_NUMPAD9 = numeric keypad 9 key, 105
	morda::key::UNKNOWN, // VK_MULTIPLY = multiply key
	morda::key::UNKNOWN, // VK_ADD
	morda::key::UNKNOWN, // VK_SEPARATOR
	morda::key::UNKNOWN, // VK_SUBTRACT
	morda::key::UNKNOWN, // VK_DECIMAL, 110
	morda::key::UNKNOWN, // VK_DIVIDE
	morda::key::F1, // VK_F1
	morda::key::F2, // VK_F2
	morda::key::F3, // VK_F3
	morda::key::F4, // VK_F4, 115
	morda::key::F5, // VK_F5
	morda::key::F6, // VK_F6
	morda::key::F7, // VK_F7
	morda::key::F8, // VK_F8
	morda::key::F9, // VK_F9, 120
	morda::key::F10, // VK_F10
	morda::key::F11, // VK_F11
	morda::key::F12, // VK_F12
	morda::key::UNKNOWN, // VK_F13
	morda::key::UNKNOWN, // VK_F14, 125
	morda::key::UNKNOWN, // VK_F15
	morda::key::UNKNOWN, // VK_F16
	morda::key::UNKNOWN, // VK_F17
	morda::key::UNKNOWN, // VK_F18
	morda::key::UNKNOWN, // VK_F19, 130
	morda::key::UNKNOWN, // VK_F20
	morda::key::UNKNOWN, // VK_F21
	morda::key::UNKNOWN, // VK_F22
	morda::key::UNKNOWN, // VK_F23
	morda::key::UNKNOWN, // VK_F24, 135
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned, 140
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // VK_NUMLOCK
	morda::key::UNKNOWN, // VK_SCROLL = scroll lock key, 145
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific, 150
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned, 155
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::LEFT_SHIFT, // VK_LSHIFT, 160
	morda::key::RIGHT_SHIFT, // VK_RSHIFT
	morda::key::LEFT_CONTROL, // VK_LCONTROL
	morda::key::RIGHT_CONTROL, // VK_RCONTROL
	morda::key::WINDOWS_MENU, // VK_LMENU = left menu key
	morda::key::WINDOWS_MENU, // VK_RMENU, 165
	morda::key::UNKNOWN, // VK_BROWSER_BACK
	morda::key::UNKNOWN, // VK_BROWSER_FORWARD
	morda::key::UNKNOWN, // VK_BROWSER_REFRESH
	morda::key::UNKNOWN, // VK_BROWSER_STOP
	morda::key::UNKNOWN, // VK_BROWSER_SEARCH, 170
	morda::key::UNKNOWN, // VK_BROWSER_FAVORITES
	morda::key::UNKNOWN, // VK_BROWSER_HOME
	morda::key::UNKNOWN, // VK_VOLUME_MUTE
	morda::key::UNKNOWN, // VK_VOLUME_DOWN
	morda::key::UNKNOWN, // VK_VOLUME_UP, 175
	morda::key::UNKNOWN, // VK_MEDIA_NEXT_TRACK
	morda::key::UNKNOWN, // VK_MEDIA_PREV_TRACK
	morda::key::UNKNOWN, // VK_MEDIA_STOP
	morda::key::UNKNOWN, // VK_MEDIA_PLAY_PAUSE
	morda::key::UNKNOWN, // VK_LAUNCH_MAIL, 180
	morda::key::UNKNOWN, // VK_LAUNCH_MEDIA_SELECT
	morda::key::UNKNOWN, // VK_LAUNCH_APP1
	morda::key::UNKNOWN, // VK_LAUNCH_APP2
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved, 185
	morda::key::UNKNOWN, // VK_OEM_1
	morda::key::UNKNOWN, // VK_OEM_PLUS
	morda::key::UNKNOWN, // VK_OEM_COMMA
	morda::key::UNKNOWN, // VK_OEM_MINUS
	morda::key::UNKNOWN, // VK_OEM_PERIOD, 190
	morda::key::UNKNOWN, // VK_OEM_2
	morda::key::UNKNOWN, // VK_OEM_3
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved, 195
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved, 200
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved, 205
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved, 210
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // Reserved, 215
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // VK_OEM_4
	morda::key::UNKNOWN, // VK_OEM_5, 220
	morda::key::UNKNOWN, // VK_OEM_6
	morda::key::UNKNOWN, // VK_OEM_7
	morda::key::UNKNOWN, // VK_OEM_8
	morda::key::UNKNOWN, // Reserved
	morda::key::UNKNOWN, // OEM specific, 225
	morda::key::UNKNOWN, // VK_OEM_102
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // VK_PROCESSKEY
	morda::key::UNKNOWN, // OEM specific, 230
	morda::key::UNKNOWN, // VK_PACKET
	morda::key::UNKNOWN, // Unassigned
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific, 235
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific, 240
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific
	morda::key::UNKNOWN, // OEM specific, 245
	morda::key::UNKNOWN, // VK_ATTN
	morda::key::UNKNOWN, // VK_CRSEL
	morda::key::UNKNOWN, // VK_EXSEL
	morda::key::UNKNOWN, // VK_EREOF
	morda::key::UNKNOWN, // VK_PLAY, 250
	morda::key::UNKNOWN, // VK_ZOOM
	morda::key::UNKNOWN, // VK_NONAME
	morda::key::UNKNOWN, // VK_PA1
	morda::key::UNKNOWN, // VK_OEM_CLEAR
	morda::key::UNKNOWN
};

class KeyEventUnicodeProvider : public morda::Morda::UnicodeProvider{
	char32_t c;
public:
	KeyEventUnicodeProvider(char32_t unicodeChar = 0) :
		c(unicodeChar)
	{}

	std::u32string get()const override{
		if (this->c == 0){
			return std::u32string();
		}

		return std::u32string(&this->c, 1);
	}
};



}// ~namespace


namespace{


LRESULT	CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg){
		case WM_ACTIVATE:
			if (!HIWORD(wParam)){ // Check Minimization State
				// window active
			}
			else{
				// window is no longer active
			}
			return 0;

		case WM_SYSCOMMAND:
			switch (wParam){
				case SC_SCREENSAVE: // screensaver trying to start?
				case SC_MONITORPOWER: // montor trying to enter powersave?
					return 0; // prevent from happening
			}
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_MOUSEMOVE:
		{
			auto& ww = getImpl(getWindowPimpl(mordavokne::inst()));
			if(!ww.isHovered){

				TRACKMOUSEEVENT tme = { sizeof(tme) };
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				TrackMouseEvent(&tme);

				ww.isHovered = true;

				// restore mouse cursor invisibility
				if(!ww.mouseCursorIsCurrentlyVisible){
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

				handleMouseHover(mordavokne::inst(), true, 0);
			}
			handleMouseMove(
					mordavokne::inst(),
					morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					0
				);
			return 0;
		}
		case WM_MOUSELEAVE:
		{
			auto& ww = getImpl(getWindowPimpl(mordavokne::inst()));

			// Windows hides the mouse cursor even in non-client areas of the window,
			// like caption bar and borders, so show cursor if it is hidden
			if(!ww.mouseCursorIsCurrentlyVisible){
				ShowCursor(TRUE);
			}

			ww.isHovered = false;
			handleMouseHover(mordavokne::inst(), false, 0);

			// Report mouse button up events for all pressed mouse buttons
			for(
					decltype(ww.mouseButtonState)::index_t i = 0;
					i != ww.mouseButtonState.size();
					++i
				)
			{
				if(ww.mouseButtonState.get(i)){
					ww.mouseButtonState.clear(i);
					handleMouseButton(
							mordavokne::inst(),
							false,
							morda::Vec2r(1000000, 1000000), // outside of the window
							morda::MouseButton_e(i),
							0
						);
				}
			}
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			auto& ww = getImpl(getWindowPimpl(mordavokne::inst()));
			ww.mouseButtonState.set(morda::MouseButton_e::LEFT);
			handleMouseButton(
					mordavokne::inst(),
					true,
					morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::MouseButton_e::LEFT,
					0
				);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			auto& ww = getImpl(getWindowPimpl(mordavokne::inst()));
			ww.mouseButtonState.clear(morda::MouseButton_e::LEFT);
			handleMouseButton(
					mordavokne::inst(),
					false,
					morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::MouseButton_e::LEFT,
					0
				);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			auto& ww = getImpl(getWindowPimpl(mordavokne::inst()));
			ww.mouseButtonState.set(morda::MouseButton_e::MIDDLE);
			handleMouseButton(
					mordavokne::inst(),
					true,
					morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::MouseButton_e::MIDDLE,
					0
				);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			auto& ww = getImpl(getWindowPimpl(mordavokne::inst()));
			ww.mouseButtonState.clear(morda::MouseButton_e::MIDDLE);
			handleMouseButton(
					mordavokne::inst(),
					false,
					morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::MouseButton_e::MIDDLE,
					0
				);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			auto& ww = getImpl(getWindowPimpl(mordavokne::inst()));
			ww.mouseButtonState.set(morda::MouseButton_e::RIGHT);
			handleMouseButton(
					mordavokne::inst(),
					true,
					morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::MouseButton_e::RIGHT,
					0
				);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			auto& ww = getImpl(getWindowPimpl(mordavokne::inst()));
			ww.mouseButtonState.clear(morda::MouseButton_e::RIGHT);
			handleMouseButton(
					mordavokne::inst(),
					false,
					morda::Vec2r(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::MouseButton_e::RIGHT,
					0
				);
			return 0;
		}
		case WM_MOUSEWHEEL:
			{
				unsigned short int times = HIWORD(wParam);
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

				// For some reason in WM_MOUSEWHEEL message mouse cursor position is sent in screen coordinates,
				// need to traslate those to window coordinates.
				if(ScreenToClient(hwnd, &pos) == 0){
					break;
				}

				for(unsigned i = 0; i != times; ++i){
					handleMouseButton(
							mordavokne::inst(),
							true,
							morda::Vec2r(float(pos.x), float(pos.y)),
							button,
							0
						);
					handleMouseButton(
							mordavokne::inst(),
							false,
							morda::Vec2r(float(pos.x), float(pos.y)),
							button,
							0
						);
				}
			}
			return 0;

		case WM_KEYDOWN:
		{
			morda::key key = keyCodeMap[std::uint8_t(wParam)];
			if ((lParam & 0x40000000) == 0){// ignore auto-repeated keypress event
				handleKeyEvent(mordavokne::inst(), true, key);
			}
			handleCharacterInput(mordavokne::inst(), KeyEventUnicodeProvider(), key);
			return 0;
		}
		case WM_KEYUP:
			handleKeyEvent(mordavokne::inst(), false, keyCodeMap[std::uint8_t(wParam)]);
			return 0;

		case WM_CHAR:
			switch(char32_t(wParam)){
				case U'\U00000008': // Backspace character
				case U'\U0000001b': // Escape character
				case U'\U0000000d': // Carriage return
					break;
				default:
					handleCharacterInput(mordavokne::inst(), KeyEventUnicodeProvider(char32_t(wParam)), morda::key::UNKNOWN);
					break;
			}
			return 0;
		case WM_PAINT:
			// we will redraw anyway on every cycle
			// app.Render();
			ValidateRect(hwnd, NULL);// This is to tell Windows that we have redrawn contents and WM_PAINT should go away from message queue.
			return 0;

		case WM_SIZE:
			// resize GL, LoWord=Width, HiWord=Height
			updateWindowRect(mordavokne::inst(), morda::Rectr(0, 0, float(LOWORD(lParam)), float(HIWORD(lParam))));
			return 0;

		case WM_USER:
			{
				std::unique_ptr<std::function<void()>> m(reinterpret_cast<std::function<void()>*>(lParam));
				(*m)();
			}
			return 0;

		default:
			break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

}// ~namespace


namespace{

morda::real getDotsPerInch(HDC dc){
	morda::real value = (morda::real(GetDeviceCaps(dc, HORZRES)) * 10.0f / morda::real(GetDeviceCaps(dc, HORZSIZE))
		+ morda::real(GetDeviceCaps(dc, VERTRES)) * 10.0f / morda::real(GetDeviceCaps(dc, VERTSIZE))) / 2.0f;
	value *= 2.54f;
	return value;
}

morda::real getDotsPerPt(HDC dc){
	r4::vec2ui resolution(GetDeviceCaps(dc, HORZRES), GetDeviceCaps(dc, VERTRES));
	r4::vec2ui screenSizeMm(GetDeviceCaps(dc, HORZSIZE), GetDeviceCaps(dc, VERTSIZE));

	return mordavokne::application::get_pixels_per_dp(resolution, screenSizeMm);
}

}// ~namespace

namespace{
std::string initializeStorageDir(const std::string& appName){
	CHAR path[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, path) != S_OK) {
		throw utki::Exc("failed to get user's profile directory.");
	}

	path[sizeof(path) - 1] = '\0';// null-terminate the string just in case

	std::string homeDirStr(path, strlen(path));

	ASSERT(homeDirStr.size() != 0)

	if(homeDirStr[homeDirStr.size() - 1] == '\\'){
		homeDirStr[homeDirStr.size() - 1] = '/';
	}

	if(homeDirStr[homeDirStr.size() - 1] != '/'){
		homeDirStr.append(1, '/');
	}

	homeDirStr.append(1, '.').append(appName).append(1, '/');

	papki::FSFile dir(homeDirStr);
	if(!dir.exists()){
		dir.makeDir();
	}

	return homeDirStr;
}
}

application::application(std::string&& name, const window_params& wp) :
		name(name),
		windowPimpl(utki::makeUnique<WindowWrapper>(wp)),
		gui(
				std::make_shared<mordaren::OpenGL2Renderer>(),
				getDotsPerInch(getImpl(this->windowPimpl).hdc),
				getDotsPerPt(getImpl(this->windowPimpl).hdc),
				[this](std::function<void()>&& a){
					auto& ww = getImpl(getWindowPimpl(mordavokne::inst()));
					if (PostMessage(ww.hwnd, WM_USER, 0, reinterpret_cast<LPARAM>(new std::remove_reference<decltype(a)>::type(std::move(a)))) == 0){
						throw morda::Exc("PostMessage(): failed");
					}
				}
			),
		storage_dir(initializeStorageDir(this->name)),
		curWinRect(0, 0, -1, -1)
{
	this->updateWindowRect(
			morda::Rectr(
					0,
					0,
					float(wp.dim.x),
					float(wp.dim.y)
				)
		);
}

void application::quit()noexcept{
	auto& ww = getImpl(this->windowPimpl);
	ww.quitFlag = true;
}

namespace mordavokne{

void winmain(int argc, const char** argv){
	decltype(mordavokne::create_application)* f;

	// Try GCC name mangling first
	f = reinterpret_cast<decltype(f)>(GetProcAddress(GetModuleHandle(NULL), TEXT("_ZN10mordavokne18create_applicationEiPPKc")));

	// TODO: deprecated, remove createApp() function loading
	if(!f){
		f = reinterpret_cast<decltype(f)>(GetProcAddress(GetModuleHandle(NULL), TEXT("_ZN10mordavokne9createAppEiPPKc")));
	}

	// TODO: deprecated, remove createApp() function loading
	if(!f){ // try MSVC function mangling style
		f = reinterpret_cast<decltype(f)>(GetProcAddress(
				GetModuleHandle(NULL),
#if M_CPU == M_CPU_X86_64
				TEXT("?createApp@mordavokne@@YA?AV?$unique_ptr@Vapplication@mordavokne@@U?$default_delete@Vapplication@mordavokne@@@std@@@std@@HPEAPEBD@Z")
#else
				TEXT("?createApp@mordavokne@@YA?AV?$unique_ptr@Vapplication@mordavokne@@U?$default_delete@Vapplication@mordavokne@@@std@@@std@@HPAPBD@Z")
#endif
			));
	}

	if(!f){ // try MSVC function mangling style
		f = reinterpret_cast<decltype(f)>(GetProcAddress(
				GetModuleHandle(NULL),
#if M_CPU == M_CPU_X86_64
				TEXT("?create_application@mordavokne@@YA?AV?$unique_ptr@Vapplication@mordavokne@@U?$default_delete@Vapplication@mordavokne@@@std@@@std@@HPEAPEBD@Z")
#else
				TEXT("?create_application@mordavokne@@YA?AV?$unique_ptr@Vapplication@mordavokne@@U?$default_delete@Vapplication@mordavokne@@@std@@@std@@HPAPBD@Z")
#endif
			));
	}

	ASSERT_INFO(f, "no app factory function found")
	std::unique_ptr<mordavokne::application> app = f(argc, argv);

	ASSERT(app)

	auto& ww = getImpl(getWindowPimpl(*app));

	ShowWindow(ww.hwnd, SW_SHOW);

	while (!ww.quitFlag){
		std::uint32_t timeout = app->gui.update();
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
					ww.quitFlag = true;
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		render(*app);
		//		TRACE(<< "loop" << std::endl)
	}
}

}



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


void application::set_fullscreen(bool enable) {
	if (enable == this->is_fullscreen()) {
		return;
	}

	auto& ww = getImpl(this->windowPimpl);

	if (enable) {
		// save original window size
		RECT rect;
		if (GetWindowRect(ww.hwnd, &rect) == 0) {
			throw utki::Exc("Failed to get window rect");
		}
		this->beforeFullScreenWindowRect.p.x = rect.left;
		this->beforeFullScreenWindowRect.p.y = rect.top;
		this->beforeFullScreenWindowRect.d.x = rect.right - rect.left;
		this->beforeFullScreenWindowRect.d.y = rect.bottom - rect.top;

		// Set new window style
		SetWindowLong(
				ww.hwnd,
				GWL_STYLE,
				GetWindowLong(ww.hwnd, GWL_STYLE)
						& ~(WS_CAPTION | WS_THICKFRAME)
			);
		SetWindowLong(
				ww.hwnd,
				GWL_EXSTYLE,
				GetWindowLong(ww.hwnd, GWL_EXSTYLE)
						& ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)
			);

		// set new window size and position
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(MonitorFromWindow(ww.hwnd, MONITOR_DEFAULTTONEAREST), &mi);
		SetWindowPos(
				ww.hwnd,
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
				ww.hwnd,
				GWL_STYLE,
				GetWindowLong(ww.hwnd, GWL_STYLE)
						| (WS_CAPTION | WS_THICKFRAME)
			);
		SetWindowLong(
				ww.hwnd,
				GWL_EXSTYLE,
				GetWindowLong(ww.hwnd, GWL_EXSTYLE)
						| (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)
			);

			SetWindowPos(
					ww.hwnd,
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


void application::set_mouse_cursor_visible(bool visible){
	auto& ww = getImpl(this->windowPimpl);

	if(visible){
		if(!ww.mouseCursorIsCurrentlyVisible){
			ShowCursor(TRUE);
			ww.mouseCursorIsCurrentlyVisible = true;
		}
	}else{
		if(ww.mouseCursorIsCurrentlyVisible){
			ShowCursor(FALSE);
			ww.mouseCursorIsCurrentlyVisible = false;
		}
	}
}


void application::swapFrameBuffers(){
	auto& ww = getImpl(this->windowPimpl);
	SwapBuffers(ww.hdc);
}



namespace{
WindowWrapper::WindowWrapper(const window_params& wp){
	this->windowClassName = "MordavokneWindowClassName";

	{
		WNDCLASS wc;
		memset(&wc, 0, sizeof(wc));

		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;// Redraw on resize, own DC for window
		wc.lpfnWndProc = (WNDPROC)wndProc;
		wc.cbClsExtra = 0;// no extra window data
		wc.cbWndExtra = 0;// no extra window data
		wc.hInstance = GetModuleHandle(NULL);// instance handle
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;// no background required for OpenGL
		wc.lpszMenuName = NULL;// we don't want a menu
		wc.lpszClassName = this->windowClassName.c_str();// Set the window class Name

		if (!RegisterClass(&wc)){
			throw morda::Exc("Failed to register window class");
		}
	}

	utki::ScopeExit scopeExitWindowClass([this](){
		if(!UnregisterClass(this->windowClassName.c_str(), GetModuleHandle(NULL))){
			ASSERT_INFO(false, "Failed to unregister window class")
		}
	});


	this->hwnd = CreateWindowEx(
			WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, // extended style
			this->windowClassName.c_str(),
			"morda app",
			WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0, // x
			0, // y
			wp.dim.x + 2 * GetSystemMetrics(SM_CXSIZEFRAME),
			wp.dim.y + GetSystemMetrics(SM_CYCAPTION) + 2 * GetSystemMetrics(SM_CYSIZEFRAME),
			NULL, // no parent window
			NULL, // no menu
			GetModuleHandle(NULL),
			NULL // do not pass anything to WM_CREATE
		);

	if (!this->hwnd){
		throw morda::Exc("Failed to create a window");
	}

	utki::ScopeExit scopeExitHwnd([this](){
		if (!DestroyWindow(this->hwnd)){
			ASSERT_INFO(false, "Failed to destroy window")
		}
	});

	// NOTE: window will be shown later, right before entering main loop and after all initial App data is initialized

	this->hdc = GetDC(this->hwnd);
	if (!this->hdc){
		throw morda::Exc("Failed to create a OpenGL device context");
	}

	utki::ScopeExit scopeExitHdc([this](){
		if (!ReleaseDC(this->hwnd, this->hdc)){
			ASSERT_INFO(false, "Failed to release device context")
		}
	});

	//	TRACE_AND_LOG(<< "application::DeviceContextWrapper::DeviceContextWrapper(): DC created" << std::endl)

	{
		static PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1, // Version number of the structure, should be 1
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			BYTE(PFD_TYPE_RGBA),
			BYTE(32), // color depth
			BYTE(0), BYTE(0), BYTE(0), BYTE(0), BYTE(0), BYTE(0), // color bits ignored
			BYTE(0), // no alpha buffer
			BYTE(0), // shift bit ignored
			BYTE(0), // no accumulation buffer
			BYTE(0), BYTE(0), BYTE(0), BYTE(0), // accumulation bits ignored
			wp.buffers.get(window_params::buffer_type::depth) ? BYTE(16) : BYTE(0), // 16bit depth buffer
			wp.buffers.get(window_params::buffer_type::stencil) ? BYTE(8) : BYTE(0),
			BYTE(0), // no auxiliary buffer
			BYTE(PFD_MAIN_PLANE), // main drawing layer
			BYTE(0), // reserved
			0, 0, 0 // layer masks ignored
		};

		int pixelFormat = ChoosePixelFormat(this->hdc, &pfd);
		if (!pixelFormat){
			throw morda::Exc("Could not find suitable pixel format");
		}

		//	TRACE_AND_LOG(<< "application::DeviceContextWrapper::DeviceContextWrapper(): pixel format chosen" << std::endl)

		if (!SetPixelFormat(this->hdc, pixelFormat, &pfd)){
			throw morda::Exc("Could not sent pixel format");
		}
	}

	this->hrc = wglCreateContext(hdc);
	if (!this->hrc) {
		throw morda::Exc("Failed to create OpenGL rendering context");
	}

	utki::ScopeExit scopeExitHrc([this](){
		if (!wglMakeCurrent(NULL, NULL)) {
			ASSERT_INFO(false, "Deactivating OpenGL rendering context failed")
		}
		if (!wglDeleteContext(this->hrc)) {
			ASSERT_INFO(false, "Releasing OpenGL rendering context failed")
		}
	});

	//	TRACE_AND_LOG(<< "application::GLContextWrapper::GLContextWrapper(): GL rendering context created" << std::endl)

	if (!wglMakeCurrent(hdc, this->hrc)) {
		throw morda::Exc("Failed to activate OpenGL rendering context");
	}

	if(glewInit() != GLEW_OK){
		throw morda::Exc("GLEW initialization failed");
	}

	scopeExitHrc.reset();
	scopeExitHdc.reset();
	scopeExitHwnd.reset();
	scopeExitWindowClass.reset();
}

WindowWrapper::~WindowWrapper()noexcept{
	if (!wglMakeCurrent(NULL, NULL)) {
		ASSERT_INFO(false, "Deactivating OpenGL rendering context failed")
	}

	if (!wglDeleteContext(this->hrc)) {
		ASSERT_INFO(false, "Releasing OpenGL rendering context failed")
	}
	if (!ReleaseDC(this->hwnd, this->hdc)){
		ASSERT_INFO(false, "Failed to release device context")
	}
	if (!DestroyWindow(this->hwnd)){
		ASSERT_INFO(false, "Failed to destroy window")
	}
	if(!UnregisterClass(this->windowClassName.c_str(), GetModuleHandle(NULL))){
		ASSERT_INFO(false, "Failed to unregister window class")
	}
}
}