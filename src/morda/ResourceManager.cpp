#include <papki/RootDirFile.hpp>

#include "ResourceManager.hpp"

#include "util/util.hpp"
#include "App.hpp"


using namespace morda;



namespace{
const char* D_Include = "include";
}



void ResourceManager::mountResPack(const papki::File& fi){
	ASSERT(!fi.isOpened())
//	TRACE(<< "fi->path() = " << fi.path() << std::endl)
	
	std::string dir = fi.dir();
	
	if(fi.notDir().size() == 0){
		fi.setPath(dir + "main.res.stob");
	}

	std::unique_ptr<stob::Node> resScript = utki::makeUnique<stob::Node>();
	resScript->setNext(stob::load(fi));
	
	//handle includes
	for(auto np = resScript->next(D_Include); np.node(); np = np.prev()->next(D_Include)){
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
	throw ResourceManager::Exc("resource name not found in mounted resource packs");
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
