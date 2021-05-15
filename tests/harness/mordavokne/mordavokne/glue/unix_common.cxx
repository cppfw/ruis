#include "../application.hpp"

namespace{

std::unique_ptr<mordavokne::application> createAppUnix(int argc, const char** argv){
	return mordavokne::application_factory::get_factory()(utki::make_span(argv, argc));
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
