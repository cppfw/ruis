#include <morda/util/util.hpp>
#include <morda/context.hpp>

#include <utki/windows.hpp>
#include <utki/destructable.hpp>

#include <papki/fs_file.hpp>

#include <morda/render/opengl2/renderer.hpp>

#include <Shlobj.h> // needed for SHGetFolderPathA()
#include <windowsx.h> // needed for GET_X_LPARAM macro and other similar macros

#include "../../application.hpp"

#include "../friend_accessors.cxx"

using namespace mordavokne;

namespace{
struct WindowWrapper : public utki::destructable{
	std::string windowClassName;
	HWND hwnd;
	HDC hdc;
	HGLRC hrc;

	bool quitFlag = false;

	bool isHovered = false; // for tracking when mouse enters or leaves window.

	utki::flags<morda::mouse_button> mouseButtonState;

	bool mouseCursorIsCurrentlyVisible = true;

	WindowWrapper(const window_params& wp);

	~WindowWrapper()noexcept;
};

WindowWrapper& getImpl(const std::unique_ptr<utki::destructable>& pimpl){
	ASSERT(dynamic_cast<WindowWrapper*>(pimpl.get()))
	return static_cast<WindowWrapper&>(*pimpl);
}
}

namespace{
const std::array<morda::key, std::uint8_t(-1) + 1> keyCodeMap = {
	morda::key::unknown, // Undefined
	morda::key::unknown, // VK_LBUTTON
	morda::key::unknown, // VK_RBUTTON
	morda::key::unknown, // VK_CANCEL
	morda::key::unknown, // VK_MBUTTON
	morda::key::unknown, // VK_XBUTTON1, 5
	morda::key::unknown, // VK_XBUTTON2
	morda::key::unknown, // Undefined
	morda::key::backspace, // VK_BACK = backspace key
	morda::key::tabulator, // VK_TAB
	morda::key::unknown, // Reserved, 10
	morda::key::unknown, // Reserved
	morda::key::unknown, // VK_CLEAR = clear key???
	morda::key::enter, // VK_RETURN
	morda::key::unknown, // Undefined
	morda::key::unknown, // Undefined, 15
	morda::key::left_shift, // VK_SHIFT
	morda::key::left_control, // VK_CONTROL
	morda::key::left_alt, // VK_MENU = alt key
	morda::key::pause, // VK_PAUSE
	morda::key::capslock, // VK_CAPITAL = caps lock key, 20
	morda::key::unknown, // VK_KANA, VK_HANGUEL, VK_HANGUL
	morda::key::unknown, // Undefined
	morda::key::unknown, // VK_JUNJA
	morda::key::unknown, // VK_FINAL
	morda::key::unknown, // VK_HANJA, VK_KANJI, 25
	morda::key::unknown, // Undefined
	morda::key::escape, // VK_ESCAPE
	morda::key::unknown, // VK_CONVERT
	morda::key::unknown, // VK_NONCONVERT
	morda::key::unknown, // VK_ACCEPT, 30
	morda::key::unknown, // VK_MODECHANGE
	morda::key::space, // VK_SPACE = space bar key
	morda::key::page_up, // VK_PRIOR = page up key
	morda::key::page_down,// VK_NEXT = page down key
	morda::key::end, // VK_END, 35
	morda::key::home,// VK_HOME
	morda::key::left,// VK_LEFT
	morda::key::up, // VK_UP
	morda::key::right, // VK_RIGHT
	morda::key::down, // VK_DOWN, 40
	morda::key::unknown, // VK_SELECT
	morda::key::unknown, // VK_PRINT
	morda::key::unknown, // VK_EXECUTE
	morda::key::print_screen,// VK_SNAPSHOT = print screen key
	morda::key::insert, // VK_INSERT, 45
	morda::key::deletion, // VK_DELETE
	morda::key::unknown, // VK_HELP
	morda::key::zero, // 0 key
	morda::key::one, // 1 key
	morda::key::two, // 2 key, 50
	morda::key::three, // 3 key
	morda::key::four, // 4 key
	morda::key::five, // 5 key
	morda::key::six, // 6 key
	morda::key::seven, // 7 key, 55
	morda::key::eight, // 8 key
	morda::key::nine, // 9 key
	morda::key::unknown, // Undefined
	morda::key::unknown, // Undefined
	morda::key::unknown, // Undefined, 60
	morda::key::unknown, // Undefined
	morda::key::unknown, // Undefined
	morda::key::unknown, // Undefined
	morda::key::unknown, // Undefined
	morda::key::a, // A key, 65
	morda::key::b, // B key
	morda::key::c, // C key
	morda::key::d, // D key
	morda::key::e, // E key
	morda::key::f, // F key, 70
	morda::key::g, // G key
	morda::key::h, // H key
	morda::key::i, // I key
	morda::key::j, // J key
	morda::key::k, // K key, 75
	morda::key::l, // L key
	morda::key::m, // M key
	morda::key::n, // N key
	morda::key::o, // O key
	morda::key::p, // P key, 80
	morda::key::q, // Q key
	morda::key::r, // R key
	morda::key::s, // S key
	morda::key::t, // T key
	morda::key::u, // U key, 85
	morda::key::v, // V key
	morda::key::w, // W key
	morda::key::x, // X key
	morda::key::y, // Y key
	morda::key::z, // Z key, 90
	morda::key::command, // VK_LWIN = left windows key
	morda::key::command, // VK_RWIN = right windows key
	morda::key::unknown, // VK_APPS = applications key
	morda::key::unknown, // Reserved
	morda::key::unknown, // VK_SLEEP = computer sleep key, 95
	morda::key::zero, // VK_NUMPAD0 = numeric keypad 0 key
	morda::key::one, // VK_NUMPAD1 = numeric keypad 1 key
	morda::key::two, // VK_NUMPAD2 = numeric keypad 2 key
	morda::key::three, // VK_NUMPAD3 = numeric keypad 3 key
	morda::key::four, // VK_NUMPAD4 = numeric keypad 4 key, 100
	morda::key::five, // VK_NUMPAD5 = numeric keypad 5 key
	morda::key::six, // VK_NUMPAD6 = numeric keypad 6 key
	morda::key::seven, // VK_NUMPAD7 = numeric keypad 7 key
	morda::key::eight, // VK_NUMPAD8 = numeric keypad 8 key
	morda::key::nine, // VK_NUMPAD9 = numeric keypad 9 key, 105
	morda::key::unknown, // VK_MULTIPLY = multiply key
	morda::key::unknown, // VK_ADD
	morda::key::unknown, // VK_SEPARATOR
	morda::key::unknown, // VK_SUBTRACT
	morda::key::unknown, // VK_DECIMAL, 110
	morda::key::unknown, // VK_DIVIDE
	morda::key::f1, // VK_F1
	morda::key::f2, // VK_F2
	morda::key::f3, // VK_F3
	morda::key::f4, // VK_F4, 115
	morda::key::f5, // VK_F5
	morda::key::f6, // VK_F6
	morda::key::f7, // VK_F7
	morda::key::f8, // VK_F8
	morda::key::f9, // VK_F9, 120
	morda::key::f10, // VK_F10
	morda::key::f11, // VK_F11
	morda::key::f12, // VK_F12
	morda::key::unknown, // VK_F13
	morda::key::unknown, // VK_F14, 125
	morda::key::unknown, // VK_F15
	morda::key::unknown, // VK_F16
	morda::key::unknown, // VK_F17
	morda::key::unknown, // VK_F18
	morda::key::unknown, // VK_F19, 130
	morda::key::unknown, // VK_F20
	morda::key::unknown, // VK_F21
	morda::key::unknown, // VK_F22
	morda::key::unknown, // VK_F23
	morda::key::unknown, // VK_F24, 135
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned, 140
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // VK_NUMLOCK
	morda::key::unknown, // VK_SCROLL = scroll lock key, 145
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific, 150
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned, 155
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::left_shift, // VK_LSHIFT, 160
	morda::key::right_shift, // VK_RSHIFT
	morda::key::left_control, // VK_LCONTROL
	morda::key::right_control, // VK_RCONTROL
	morda::key::menu, // VK_LMENU = left menu key
	morda::key::menu, // VK_RMENU, 165
	morda::key::unknown, // VK_BROWSER_BACK
	morda::key::unknown, // VK_BROWSER_FORWARD
	morda::key::unknown, // VK_BROWSER_REFRESH
	morda::key::unknown, // VK_BROWSER_STOP
	morda::key::unknown, // VK_BROWSER_SEARCH, 170
	morda::key::unknown, // VK_BROWSER_FAVORITES
	morda::key::unknown, // VK_BROWSER_HOME
	morda::key::unknown, // VK_VOLUME_MUTE
	morda::key::unknown, // VK_VOLUME_DOWN
	morda::key::unknown, // VK_VOLUME_UP, 175
	morda::key::unknown, // VK_MEDIA_NEXT_TRACK
	morda::key::unknown, // VK_MEDIA_PREV_TRACK
	morda::key::unknown, // VK_MEDIA_STOP
	morda::key::unknown, // VK_MEDIA_PLAY_PAUSE
	morda::key::unknown, // VK_LAUNCH_MAIL, 180
	morda::key::unknown, // VK_LAUNCH_MEDIA_SELECT
	morda::key::unknown, // VK_LAUNCH_APP1
	morda::key::unknown, // VK_LAUNCH_APP2
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved, 185
	morda::key::unknown, // VK_OEM_1
	morda::key::unknown, // VK_OEM_PLUS
	morda::key::unknown, // VK_OEM_COMMA
	morda::key::unknown, // VK_OEM_MINUS
	morda::key::unknown, // VK_OEM_PERIOD, 190
	morda::key::unknown, // VK_OEM_2
	morda::key::unknown, // VK_OEM_3
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved, 195
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved, 200
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved, 205
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved, 210
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved
	morda::key::unknown, // Reserved, 215
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // Unassigned
	morda::key::unknown, // VK_OEM_4
	morda::key::unknown, // VK_OEM_5, 220
	morda::key::unknown, // VK_OEM_6
	morda::key::unknown, // VK_OEM_7
	morda::key::unknown, // VK_OEM_8
	morda::key::unknown, // Reserved
	morda::key::unknown, // OEM specific, 225
	morda::key::unknown, // VK_OEM_102
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // VK_PROCESSKEY
	morda::key::unknown, // OEM specific, 230
	morda::key::unknown, // VK_PACKET
	morda::key::unknown, // Unassigned
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific, 235
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific, 240
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific
	morda::key::unknown, // OEM specific, 245
	morda::key::unknown, // VK_ATTN
	morda::key::unknown, // VK_CRSEL
	morda::key::unknown, // VK_EXSEL
	morda::key::unknown, // VK_EREOF
	morda::key::unknown, // VK_PLAY, 250
	morda::key::unknown, // VK_ZOOM
	morda::key::unknown, // VK_NONAME
	morda::key::unknown, // VK_PA1
	morda::key::unknown, // VK_OEM_CLEAR
	morda::key::unknown
};

class KeyEventUnicodeProvider : public morda::gui::unicode_provider{
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
}

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
			auto& ww = getImpl(get_window_pimpl(mordavokne::inst()));
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
			handle_mouse_move(
					mordavokne::inst(),
					morda::vector2(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					0
				);
			return 0;
		}
		case WM_MOUSELEAVE:
		{
			auto& ww = getImpl(get_window_pimpl(mordavokne::inst()));

			// Windows hides the mouse cursor even in non-client areas of the window,
			// like caption bar and borders, so show cursor if it is hidden
			if(!ww.mouseCursorIsCurrentlyVisible){
				ShowCursor(TRUE);
			}

			ww.isHovered = false;
			handleMouseHover(mordavokne::inst(), false, 0);

			// Report mouse button up events for all pressed mouse buttons
			for(
					size_t i = 0;
					i != ww.mouseButtonState.size();
					++i
				)
			{
				auto btn = morda::mouse_button(i);
				if(ww.mouseButtonState.get(btn)){
					ww.mouseButtonState.clear(btn);
					handle_mouse_button(
							mordavokne::inst(),
							false,
							morda::vector2(1000000, 1000000), // outside of the window
							btn,
							0
						);
				}
			}
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			auto& ww = getImpl(get_window_pimpl(mordavokne::inst()));
			ww.mouseButtonState.set(morda::mouse_button::left);
			handle_mouse_button(
					mordavokne::inst(),
					true,
					morda::vector2(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::mouse_button::left,
					0
				);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			auto& ww = getImpl(get_window_pimpl(mordavokne::inst()));
			ww.mouseButtonState.clear(morda::mouse_button::left);
			handle_mouse_button(
					mordavokne::inst(),
					false,
					morda::vector2(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::mouse_button::left,
					0
				);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			auto& ww = getImpl(get_window_pimpl(mordavokne::inst()));
			ww.mouseButtonState.set(morda::mouse_button::middle);
			handle_mouse_button(
					mordavokne::inst(),
					true,
					morda::vector2(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::mouse_button::middle,
					0
				);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			auto& ww = getImpl(get_window_pimpl(mordavokne::inst()));
			ww.mouseButtonState.clear(morda::mouse_button::middle);
			handle_mouse_button(
					mordavokne::inst(),
					false,
					morda::vector2(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::mouse_button::middle,
					0
				);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			auto& ww = getImpl(get_window_pimpl(mordavokne::inst()));
			ww.mouseButtonState.set(morda::mouse_button::right);
			handle_mouse_button(
					mordavokne::inst(),
					true,
					morda::vector2(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::mouse_button::right,
					0
				);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			auto& ww = getImpl(get_window_pimpl(mordavokne::inst()));
			ww.mouseButtonState.clear(morda::mouse_button::right);
			handle_mouse_button(
					mordavokne::inst(),
					false,
					morda::vector2(float(GET_X_LPARAM(lParam)), float(GET_Y_LPARAM(lParam))),
					morda::mouse_button::right,
					0
				);
			return 0;
		}
		case WM_MOUSEWHEEL:
			{
				unsigned short int times = HIWORD(wParam);
				times /= 120;
				morda::mouse_button button;
				if(times >= 0){
					button = morda::mouse_button::wheel_up;
				}else{
					times = -times;
					button = morda::mouse_button::wheel_down;
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
					handle_mouse_button(
							mordavokne::inst(),
							true,
							morda::vector2(float(pos.x), float(pos.y)),
							button,
							0
						);
					handle_mouse_button(
							mordavokne::inst(),
							false,
							morda::vector2(float(pos.x), float(pos.y)),
							button,
							0
						);
				}
			}
			return 0;

		case WM_KEYDOWN:
		{
			morda::key key = keyCodeMap[std::uint8_t(wParam)];
			if ((lParam & 0x40000000) == 0){ // ignore auto-repeated keypress event
				handle_key_event(mordavokne::inst(), true, key);
			}
			handle_character_input(mordavokne::inst(), KeyEventUnicodeProvider(), key);
			return 0;
		}
		case WM_KEYUP:
			handle_key_event(mordavokne::inst(), false, keyCodeMap[std::uint8_t(wParam)]);
			return 0;

		case WM_CHAR:
			switch(char32_t(wParam)){
				case U'\U00000008': // Backspace character
				case U'\U0000001b': // Escape character
				case U'\U0000000d': // Carriage return
					break;
				default:
					handle_character_input(mordavokne::inst(), KeyEventUnicodeProvider(char32_t(wParam)), morda::key::unknown);
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
			update_window_rect(mordavokne::inst(), morda::rectangle(0, 0, float(LOWORD(lParam)), float(HIWORD(lParam))));
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
}

namespace{
morda::real getDotsPerInch(HDC dc){
	morda::real value = (morda::real(GetDeviceCaps(dc, HORZRES)) * 10.0f / morda::real(GetDeviceCaps(dc, HORZSIZE))
		+ morda::real(GetDeviceCaps(dc, VERTRES)) * 10.0f / morda::real(GetDeviceCaps(dc, VERTSIZE))) / 2.0f;
	value *= 2.54f;
	return value;
}
}

namespace{
morda::real getDotsPerPt(HDC dc){
	r4::vector2<unsigned> resolution(GetDeviceCaps(dc, HORZRES), GetDeviceCaps(dc, VERTRES));
	r4::vector2<unsigned> screenSizeMm(GetDeviceCaps(dc, HORZSIZE), GetDeviceCaps(dc, VERTSIZE));

	return mordavokne::application::get_pixels_per_dp(resolution, screenSizeMm);
}
}

namespace{
std::string initialize_storage_dir(const std::string& appName){
	CHAR path[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, path) != S_OK) {
		throw std::runtime_error("failed to get user's profile directory.");
	}

	path[sizeof(path) - 1] = '\0'; // null-terminate the string just in case

	std::string homeDirStr(path, strlen(path));

	ASSERT(homeDirStr.size() != 0)

	if(homeDirStr[homeDirStr.size() - 1] == '\\'){
		homeDirStr[homeDirStr.size() - 1] = '/';
	}

	if(homeDirStr[homeDirStr.size() - 1] != '/'){
		homeDirStr.append(1, '/');
	}

	homeDirStr.append(1, '.').append(appName).append(1, '/');

	papki::fs_file dir(homeDirStr);
	if(!dir.exists()){
		dir.make_dir();
	}

	return homeDirStr;
}
}

application::application(std::string&& name, const window_params& wp) :
		name(name),
		window_pimpl(std::make_unique<WindowWrapper>(wp)),
		gui(std::make_shared<morda::context>(
				std::make_shared<morda::render_opengl2::renderer>(),
				std::make_shared<morda::updater>(),
				[this](std::function<void()>&& a){
					auto& ww = getImpl(get_window_pimpl(mordavokne::inst()));
					if (PostMessage(ww.hwnd, WM_USER, 0, reinterpret_cast<LPARAM>(new std::remove_reference<decltype(a)>::type(std::move(a)))) == 0){
						throw std::runtime_error("PostMessage(): failed");
					}
				},
				[this](morda::mouse_cursor c){
					// TODO:
				},
				getDotsPerInch(getImpl(this->window_pimpl).hdc),
				getDotsPerPt(getImpl(this->window_pimpl).hdc)
			)),
		storage_dir(initialize_storage_dir(this->name)),
		curWinRect(0, 0, -1, -1)
{
	this->update_window_rect(
			morda::rectangle(
					0,
					0,
					float(wp.dim.x()),
					float(wp.dim.y())
				)
		);
}

void application::quit()noexcept{
	auto& ww = getImpl(this->window_pimpl);
	ww.quitFlag = true;
}

namespace mordavokne{
void winmain(int argc, const char** argv){
	auto app = mordavokne::application_factory::get_factory()(utki::make_span(argv, argc));
	if(!app){
		return;
	}

	ASSERT(app)

	auto& ww = getImpl(get_window_pimpl(*app));

	ShowWindow(ww.hwnd, SW_SHOW);

	while (!ww.quitFlag){
		uint32_t timeout = app->gui.update();
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

void application::set_fullscreen(bool enable){
	if(enable == this->is_fullscreen()){
		return;
	}

	auto& ww = getImpl(this->window_pimpl);

	if(enable){
		// save original window size
		RECT rect;
		if(GetWindowRect(ww.hwnd, &rect) == 0){
			throw std::runtime_error("Failed to get window rect");
		}
		this->beforeFullScreenWindowRect.p.x() = rect.left;
		this->beforeFullScreenWindowRect.p.y() = rect.top;
		this->beforeFullScreenWindowRect.d.x() = rect.right - rect.left;
		this->beforeFullScreenWindowRect.d.y() = rect.bottom - rect.top;

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
					this->beforeFullScreenWindowRect.p.x(),
					this->beforeFullScreenWindowRect.p.y(),
					this->beforeFullScreenWindowRect.d.x(),
					this->beforeFullScreenWindowRect.d.y(),
					SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
				);
	}

	this->isFullscreen_v = enable;
}

void application::set_mouse_cursor_visible(bool visible){
	auto& ww = getImpl(this->window_pimpl);

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

void application::swap_frame_buffers(){
	auto& ww = getImpl(this->window_pimpl);
	SwapBuffers(ww.hdc);
}

namespace{
WindowWrapper::WindowWrapper(const window_params& wp){
	this->windowClassName = "MordavokneWindowClassName";

	{
		WNDCLASS wc;
		memset(&wc, 0, sizeof(wc));

		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // redraw on resize, own DC for window
		wc.lpfnWndProc = (WNDPROC)wndProc;
		wc.cbClsExtra = 0; // no extra window data
		wc.cbWndExtra = 0; // no extra window data
		wc.hInstance = GetModuleHandle(NULL); // instance handle
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL; // no background required for OpenGL
		wc.lpszMenuName = NULL; // we don't want a menu
		wc.lpszClassName = this->windowClassName.c_str(); // set the window class Name

		if (!RegisterClass(&wc)){
			throw std::runtime_error("Failed to register window class");
		}
	}

	utki::scope_exit scopeExitWindowClass([this](){
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
			wp.dim.x() + 2 * GetSystemMetrics(SM_CXSIZEFRAME),
			wp.dim.y() + GetSystemMetrics(SM_CYCAPTION) + 2 * GetSystemMetrics(SM_CYSIZEFRAME),
			NULL, // no parent window
			NULL, // no menu
			GetModuleHandle(NULL),
			NULL // do not pass anything to WM_CREATE
		);

	if (!this->hwnd){
		throw std::runtime_error("Failed to create a window");
	}

	utki::scope_exit scopeExitHwnd([this](){
		if (!DestroyWindow(this->hwnd)){
			ASSERT_INFO(false, "Failed to destroy window")
		}
	});

	// NOTE: window will be shown later, right before entering main loop and after all initial App data is initialized

	this->hdc = GetDC(this->hwnd);
	if (!this->hdc){
		throw std::runtime_error("Failed to create a OpenGL device context");
	}

	utki::scope_exit scopeExitHdc([this](){
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
			throw std::runtime_error("Could not find suitable pixel format");
		}

		//	TRACE_AND_LOG(<< "application::DeviceContextWrapper::DeviceContextWrapper(): pixel format chosen" << std::endl)

		if (!SetPixelFormat(this->hdc, pixelFormat, &pfd)){
			throw std::runtime_error("Could not sent pixel format");
		}
	}

	this->hrc = wglCreateContext(hdc);
	if (!this->hrc) {
		throw std::runtime_error("Failed to create OpenGL rendering context");
	}

	utki::scope_exit scopeExitHrc([this](){
		if (!wglMakeCurrent(NULL, NULL)) {
			ASSERT_INFO(false, "Deactivating OpenGL rendering context failed")
		}
		if (!wglDeleteContext(this->hrc)) {
			ASSERT_INFO(false, "Releasing OpenGL rendering context failed")
		}
	});

	//	TRACE_AND_LOG(<< "application::GLContextWrapper::GLContextWrapper(): GL rendering context created" << std::endl)

	if (!wglMakeCurrent(hdc, this->hrc)) {
		throw std::runtime_error("Failed to activate OpenGL rendering context");
	}

	if(glewInit() != GLEW_OK){
		throw std::runtime_error("GLEW initialization failed");
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
