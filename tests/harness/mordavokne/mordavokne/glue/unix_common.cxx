#include <dlfcn.h>

#include "../application.hpp"

namespace{

std::unique_ptr<mordavokne::application> createAppUnix(int argc, const char** argv){
	void* libHandle = dlopen(nullptr, RTLD_NOW);
	if(!libHandle){
		throw std::runtime_error("dlopen(): failed");
	}

	utki::scope_exit scopeExit([libHandle](){
		dlclose(libHandle);
	});

	auto factory = reinterpret_cast<decltype(mordavokne::create_application)*>(
			dlsym(libHandle, "_ZN10mordavokne18create_applicationEiPPKc")
		);

	if(!factory){
		throw std::runtime_error("dlsym(): mordavokne::create_application() function not found!");
	}

	return factory(argc, argv);
}

std::string initialize_storage_dir(const std::string& appName){
	auto homeDir = getenv("HOME");
	if(!homeDir){
		throw std::runtime_error("failed to get user home directory. Is HOME environment variable set?");
	}

	std::string homeDirStr(homeDir);

	if(*homeDirStr.rend() != '/'){
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
