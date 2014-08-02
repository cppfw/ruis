//This file contains implementations of platform dependent methods from App class.

#include "../../App.hpp"

#include <windows.h>

#include <ting/Ptr.hpp>
#include <ting/fs/FSFile.hpp>



namespace morda{

void Main(int argc, const char** argv){
	typedef std::unique_ptr<morda::App>(*Factory)(int, const char**, const ting::Buffer<const ting::u8>&);

	Factory f;
	
	f = reinterpret_cast<Factory>(GetProcAddress(GetModuleHandle(NULL), TEXT("_ZN5morda9CreateAppEiPPKcRKN4ting6BufferIKhEE")));

	if(!f){ //try MSVC function mangling style
		f = reinterpret_cast<Factory>(GetProcAddress(GetModuleHandle(NULL), TEXT("?CreateApp@morda@@YA?AV?$unique_ptr@VApp@morda@@U?$default_delete@VApp@morda@@@std@@@std@@HPAPBDABV?$Buffer@$$CBE@ting@@@Z")));
	}

	std::unique_ptr<morda::App> a = f(argc, argv, ting::Buffer<const ting::u8>(0, 0));
	
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

