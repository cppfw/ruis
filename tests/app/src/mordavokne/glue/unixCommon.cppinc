#include <dlfcn.h>

#include "../AppFactory.hpp"

namespace{

std::unique_ptr<mordavokne::App> createAppUnix(int argc, const char** argv){
	void* libHandle = dlopen(nullptr, RTLD_NOW);
	if(!libHandle){
		throw morda::Exc("dlopen(): failed");
	}

	utki::ScopeExit scopeExit([libHandle](){
		dlclose(libHandle);
	});

	auto factory = reinterpret_cast<decltype(mordavokne::createApp)*>(
			dlsym(libHandle, "_ZN10mordavokne9createAppEiPPKc")
		);
									
	if(!factory){
		throw morda::Exc("dlsym(): createApp() function not found!");
	}

	return factory(argc, argv);
}

std::string initializeStorageDir(const std::string& appName){
	auto homeDir = getenv("HOME");
	if(!homeDir){
		throw utki::Exc("failed to get user home directory. Is HOME environment variable set?");
	}
	
	std::string homeDirStr(homeDir);
	
	if(*homeDirStr.rend() != '/'){
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
