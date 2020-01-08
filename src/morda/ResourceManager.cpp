#include <papki/RootDirFile.hpp>

#include "ResourceManager.hpp"

#include "util/util.hpp"



using namespace morda;



namespace{
const char* include_c = "include";
const char* includeSubdirs_c = "includeSubdirs";
}



void ResourceManager::mountResPack(const papki::File& fi){
	ASSERT(!fi.isOpened())
//	TRACE(<< "ResourceManager::mountResPack(): fi->path() = " << fi.path() << std::endl)
	
	std::string dir = fi.dir();
	
	if(fi.notDir().size() == 0){
		fi.setPath(dir + "main.res");
	}

	std::unique_ptr<stob::Node> resScript = utki::makeUnique<stob::Node>();
	resScript->setNext(stob::load(fi));
	ASSERT(!fi.isOpened())
	//handle includeSubdirs
	if(resScript->next(includeSubdirs_c).get_node()){
//		TRACE(<< "includeSubdirs encountered!!!!!!!!!!!!!!!" << std::endl)
		fi.setPath(fi.dir());
		auto dirContents = fi.list_dir();
		ASSERT(!fi.isOpened())
		for(auto& fileName : dirContents){
			if(fileName.size() != 0 && fileName[fileName.size() - 1] == '/'){
				fi.setPath(dir + fileName);
//				TRACE(<< "mounting respack " << fi.path() << std::endl)
				this->mountResPack(fi);
			}
		}
	}
	
	//handle includes
	for(auto np = resScript->next(include_c); np.get_node(); np = np.prev()->next(include_c)){
		ASSERT(np.prev())
		auto incNode = np.prev()->removeNext()->removeChildren();
		
		fi.setPath(dir + incNode->value());
		this->mountResPack(fi);
	}

	if(!resScript->next()){
		return;
	}
	
	ResPackEntry rpe;
	rpe.fi = papki::RootDirFile::makeUniqueConst(fi.spawn(), dir);
	rpe.resScript = resScript->chopNext();

	this->resPacks.push_back(std::move(rpe));
	ASSERT(this->resPacks.back().fi)
	ASSERT(this->resPacks.back().resScript)
}



ResourceManager::FindInScriptRet ResourceManager::findResourceInScript(const std::string& resName){
//	TRACE(<< "ResourceManager::FindResourceInScript(): resName = " << (resName.c_str()) << std::endl)

	for(auto i = this->resPacks.rbegin(); i != this->resPacks.rend(); ++i){
		for(const stob::Node* e = i->resScript.operator->(); e; e = e->next()){
			if(resName.compare(e->value()) == 0){
//				TRACE(<< "ResourceManager::FindResourceInScript(): resource found" << std::endl)
				return FindInScriptRet(*i, *e);
			}
		}//~for(res)
	}//~for(resPack)
	TRACE(<< "resource name not found in mounted resource packs: " << resName << std::endl)
	std::stringstream ss;
	ss << "resource name not found in mounted resource packs: " << resName;
	throw ResourceManager::Exc(ss.str());
}



void ResourceManager::addResource(const std::shared_ptr<Resource>& res, const stob::Node& node){
	ASSERT(res)

	ASSERT(this->resMap.find(node.value()) == this->resMap.end())
	
	//add the resource to the resources map of ResMan
	auto result = this->resMap.insert(
			std::pair<const char*, std::weak_ptr<Resource>>(
					node.value(),
					std::weak_ptr<Resource>(res)
				)
		);
	if(!result.second){
		ASSERT(false)
	}
	
//#ifdef DEBUG
//	for(T_ResMap::iterator i = this->resMap->rm.begin(); i != this->resMap->rm.end(); ++i){
//		TRACE(<< "\t" << *(*i).first << std::endl)
//	}
//#endif
}
