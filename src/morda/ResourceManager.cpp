#include <ting/fs/RootDirFile.hpp>

#include "ResourceManager.hpp"

#include "util/util.hpp"
#include "App.hpp"


using namespace morda;



namespace{
const char* D_Include = "include";
}



void ResourceManager::MountResPack(const ting::fs::File& fi){
	ASSERT(!fi.IsOpened())
	
	std::string dir = fi.Dir();
	
	if(fi.NotDir().size() == 0){
		fi.SetPath(dir + "main.res.stob");
	}

	std::unique_ptr<stob::Node> resScript = stob::Node::New();
	resScript->SetNext(stob::Load(fi));
	
	//handle includes
	for(stob::Node::NodeAndPrev np = resScript->Next(D_Include); np.node(); np = np.node()->Next(D_Include)){
		ASSERT(np.prev())
		auto incNode = np.prev()->RemoveNext()->RemoveChildren();
		
		fi.SetPath(dir + incNode->Value());
		this->MountResPack(fi);
	}

	if(!resScript->Next()){
		return;
	}
	
	ResPackEntry rpe;
	rpe.fi = ting::fs::RootDirFile::NewConst(fi.Spawn(), dir);
	rpe.resScript = resScript->ChopNext();

	this->resPacks.push_back(std::move(rpe));
	ASSERT(this->resPacks.back().fi)
	ASSERT(this->resPacks.back().resScript)
}



ResourceManager::FindInScriptRet ResourceManager::FindResourceInScript(const std::string& resName){
//	TRACE(<< "ResourceManager::FindResourceInScript(): resName = " << (resName.c_str()) << std::endl)

	for(auto i = this->resPacks.rbegin(); i != this->resPacks.rend(); ++i){
		for(const stob::Node* e = i->resScript.operator->(); e; e = e->Next()){
			if(resName.compare(e->Value()) == 0){
//				TRACE(<< "ResourceManager::FindResourceInScript(): resource found" << std::endl)
				return FindInScriptRet(*i, *e);
			}
		}//~for(res)
	}//~for(resPack)
	TRACE(<< "resource name not found in mounted resource packs: " << resName << std::endl)
	throw ResourceManager::Exc("resource name not found in mounted resource packs");
}



void ResourceManager::AddResource(const std::shared_ptr<Resource>& res, const stob::Node& node){
	ASSERT(res)

	ASSERT(this->resMap.find(node.Value()) == this->resMap.end())
	
	//add the resource to the resources map of ResMan
	auto result = this->resMap.insert(
			std::pair<const char*, std::weak_ptr<Resource>>(
					node.Value(),
					std::move(std::weak_ptr<Resource>(res))
				)
		);
	ASSERT(result.second)

	res->resMapIter = result.first;
	
//#ifdef DEBUG
//	for(T_ResMap::iterator i = this->resMap->rm.begin(); i != this->resMap->rm.end(); ++i){
//		TRACE(<< "\t" << *(*i).first << std::endl)
//	}
//#endif
}



Resource::~Resource()NOEXCEPT{
	morda::App::Inst().resMan.resMap.erase(this->resMapIter);
}

