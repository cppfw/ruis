#include "../App.hpp"

#include <sstream>

#include <ting/fs/FSFile.hpp>

using namespace morda;



void App::MountDefaultResPack(){
#ifndef DEBUG
	unsigned soname = 
#include "../../soname.txt"
		;
#endif
		
	std::array<std::string, 2> paths = {
#ifdef DEBUG
		"../../res/"
#else
		static_cast<std::stringstream&>(std::stringstream() << "/usr/local/share/morda/res" << soname << "/").str(),
		static_cast<std::stringstream&>(std::stringstream() << "/usr/share/morda/res" << soname << "/").str()
#endif
	};

	for(const auto& s : paths){
		try{
			ting::fs::FSFile fi(s);
			this->resMan.MountResPack(fi);
		}catch(ting::fs::File::Exc& e){
			continue;
		}
		break;
	}
}
