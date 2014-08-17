//This file contains implementations of platform dependent methods from App class.

#include "../../App.hpp"

#include <ting/windows.hpp>

#include <ting/fs/FSFile.hpp>



namespace morda{

void Main(int argc, const char** argv){
	typedef std::unique_ptr<morda::App>(*Factory)(int, const char**, const ting::Buffer<const std::uint8_t>&);

	Factory f;
	
	//Try GCC name mangling first
	f = reinterpret_cast<Factory>(GetProcAddress(GetModuleHandle(NULL), TEXT("_ZN5morda9CreateAppEiPPKcN4ting6BufferIKhEE")));

	if(!f){ //try MSVC function mangling style
		f = reinterpret_cast<Factory>(GetProcAddress(GetModuleHandle(NULL), TEXT("?CreateApp@morda@@YA?AV?$unique_ptr@VApp@morda@@U?$default_delete@VApp@morda@@@std@@@std@@HPAPBDV?$Buffer@$$CBE@ting@@@Z")));
	}

	ASSERT(f)
	std::unique_ptr<morda::App> a = f(argc, argv, ting::Buffer<const std::uint8_t>(0, 0));
	
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

