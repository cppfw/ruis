#include <papki/root_dir.hpp>
#include <papki/util.hpp>

#include "resource_loader.hpp"

#include "util/util.hpp"

using namespace morda;

namespace{
const char* wording_include = "include";
const char* wording_include_subdirs = "include_subdirs";
}

void resource_loader::mount_res_pack(const papki::file& fi){
	ASSERT(!fi.is_open())
	
	std::string dir = fi.dir();
	
	if(fi.not_dir().size() == 0){
		fi.set_path(dir + "main.res");
	}

	auto script = puu::read(fi);
	ASSERT(!fi.is_open())

	// handle includes
	for(auto& p : script){
		if(p.value == wording_include){
			fi.set_path(dir + get_property_value(p).to_string());
			this->mount_res_pack(fi);
			// TODO: remove "include" tree from the forest?
		}else if(p.value == wording_include_subdirs){
			fi.set_path(fi.dir());
			for(auto& f : fi.list_dir()){
				if(papki::is_dir(f)){
					fi.set_path(dir + f);
					this->mount_res_pack(fi);
				}
			}
			// TODO: remove "include_subdirs" tree from the forest?
		}
	}
	
	ResPackEntry rpe;
	rpe.fi = papki::root_dir::make(fi.spawn(), dir);
	rpe.script = std::move(script);

	this->resPacks.push_back(std::move(rpe));
	ASSERT(this->resPacks.back().fi)
	ASSERT(!this->resPacks.back().script.empty())
}

resource_loader::FindInScriptRet resource_loader::findResourceInScript(const std::string& resName){
	for(auto i = this->resPacks.rbegin(); i != this->resPacks.rend(); ++i){
		auto j = std::find(i->script.begin(), i->script.end(), resName);
		if(j != i->script.end()){
			return FindInScriptRet(*i, *j);
		}
	}
	TRACE(<< "resource name not found in mounted resource packs: " << resName << std::endl)
	std::stringstream ss;
	ss << "resource name not found in mounted resource packs: " << resName;
	throw std::logic_error(ss.str());
}

void resource_loader::addResource(const std::shared_ptr<resource>& res, const std::string& name){
	ASSERT(res)

	ASSERT(this->resMap.find(name) == this->resMap.end())
	
	//add the resource to the resources map of ResMan
	this->resMap.insert(
			std::make_pair(name, std::weak_ptr<resource>(res))
		);
	
//#ifdef DEBUG
//	for(T_ResMap::iterator i = this->resMap->rm.begin(); i != this->resMap->rm.end(); ++i){
//		TRACE(<< "\t" << *(*i).first << std::endl)
//	}
//#endif
}
