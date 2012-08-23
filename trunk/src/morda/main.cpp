#include "App.hpp"


namespace morda{
inline void Main(){
	ting::Ptr<morda::App> app = morda::CreateApp();
	
	app->Init();
	
	app->Exec();
}
}


int main(int argc, char** argv){
	morda::Main();
	
	return 0;
}
