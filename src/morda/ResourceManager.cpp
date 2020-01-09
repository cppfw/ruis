#include <papki/RootDirFile.hpp>

#include "ResourceManager.hpp"

#include "util/util.hpp"



using namespace morda;



namespace{
const char* include_c = "include";
const char* includeSubdirs_c = "includeSubdirs";
}



void ResourceManager::mountResPack(const papki::file& fi){
	ASSERT(!fi.is_open())
	
	std::string dir = fi.dir();
	
	if(fi.not_dir().size() == 0){
		fi.set_path(dir + "main.res");
	}

	auto script = puu::read(fi);
	ASSERT(!fi.is_open())

	// handle includes
	for(auto& p : script){
		if(p.value == include_c){
			fi.set_path(dir + get_property_value(p).to_string());
			this->mountResPack(fi);
			// TODO: remove "include" tree from the forest?
		}else if(p.value == includeSubdirs_c){
			fi.set_path(fi.dir());
			for(auto& f : fi.list_dir()){
				// TODO: use papki::is_dir()
				if(f.length() != 0 && f[f.size() - 1] == '/'){
					fi.set_path(dir + f);
					this->mountResPack(fi);
				}
			}
			// TODO: remove "includeSubdirs" tree from the forest?
		}
	}
	
	ResPackEntry rpe;
	rpe.fi = papki::root_dir::make(fi.spawn(), dir);
	rpe.script = std::move(script);

	this->resPacks.push_back(std::move(rpe));
	ASSERT(this->resPacks.back().fi)
	ASSERT(!this->resPacks.back().script.empty())
}



ResourceManager::FindInScriptRet ResourceManager::findResourceInScript(const std::string& resName){
	for(auto i = this->resPacks.rbegin(); i != this->resPacks.rend(); ++i){
		auto j = std::find(i->script.begin(), i->script.end(), resName);
		if(j != i->script.end()){
			return FindInScriptRet(*i, *j);
		}
	}
	TRACE(<< "resource name not found in mounted resource packs: " << resName << std::endl)
	std::stringstream ss;
	ss << "resource name not found in mounted resource packs: " << resName;
	throw ResourceManager::Exc(ss.str());
}



void ResourceManager::addResource(const std::shared_ptr<Resource>& res, const std::string& name){
	ASSERT(res)

	ASSERT(this->resMap.find(name) == this->resMap.end())
	
	//add the resource to the resources map of ResMan
	auto result = this->resMap.insert(
			std::make_pair(name, std::weak_ptr<Resource>(res))
		);
	ASSERT(result.second)
	
//#ifdef DEBUG
//	for(T_ResMap::iterator i = this->resMap->rm.begin(); i != this->resMap->rm.end(); ++i){
//		TRACE(<< "\t" << *(*i).first << std::endl)
//	}
//#endif
}
