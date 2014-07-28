//This file contains implementations of platform dependent methods from App class.

#include "../../AppFactory.hpp"

#include <windows.h>

#include <ting/Ptr.hpp>
#include <ting/fs/FSFile.hpp>



namespace morda{

inline void Main(int argc, const char** argv){
	std::unique_ptr<morda::App> a = morda::CreateApp(argc, argv, ting::Buffer<const ting::u8>(0, 0));

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

