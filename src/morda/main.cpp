#include "App.hpp"


namespace morda{
inline void Main(int argc, char** argv){
	ting::Ptr<morda::App> app = morda::CreateApp(argc, argv);
	
	app->Init();
	
	app->Exec();
}
}


int main(int argc, char** argv){
	morda::Main(argc, argv);
	
	return 0;
}
