#include "ResourceManager.hpp"

#include "util/util.hpp"
#include "App.hpp"


using namespace morda;



namespace{

const char* DResTag = "Res";

}//~namespace



void ResourceManager::MountResPack(std::unique_ptr<ting::fs::File> fi){
	ASSERT(fi)
	ASSERT(!fi->IsOpened())

	ResPackEntry rpe;

	rpe.fi = std::move(fi);

	ASS(rpe.fi)->SetPath("main.res.stob");

	std::unique_ptr<stob::Node> resScript = stob::Load(*rpe.fi);

	//handle includes
	resScript = std::move(std::get<0>(ResolveIncludes(*rpe.fi, std::move(resScript))));

	rpe.resScript = std::move(resScript);

	this->resPacks.push_back(std::move(rpe));
	ASSERT(this->resPacks.back().fi)
	ASSERT(this->resPacks.back().resScript)
}



ResourceManager::FindInScriptRet ResourceManager::FindResourceInScript(const std::string& resName){
//	TRACE(<< "ResourceManager::FindResourceInScript(): resName = " << (resName.c_str()) << std::endl)

	for(T_ResPackList::iterator i = this->resPacks.begin(); i != this->resPacks.end(); ++i){
		for(const stob::Node* e = i->resScript->ThisOrNext(DResTag).node(); e; e = e->Next(DResTag).node()){
//			TRACE(<< "ResourceManager::FindResourceInScript(): searching for 'name' property" << std::endl)
			if(const stob::Node* nameProp = e->GetProperty("name")){
	//			TRACE(<< "ResourceManager::FindResourceInScript(): name = " << name << std::endl)
				if(resName.compare(nameProp->Value()) == 0){
	//				TRACE(<< "ResourceManager::FindResourceInScript(): resource found" << std::endl)
					return FindInScriptRet(*i, *e, *nameProp);
				}
			}else{
//				TRACE(<< "ResourceManager::FindResourceInScript(): WARNING! no 'name' property in resource" << std::endl)
			}
		}//~for(res)
	}//~for(resPack)
	TRACE(<< "resource name not found in mounted resource packs: " << resName << std::endl)
	throw ResourceManager::Exc("resource name not found in mounted resource packs");
}



void ResourceManager::AddResource(const ting::Ref<Resource>& res, const stob::Node& node){
	ASSERT(res)

	ASSERT(this->resMap.find(node.Value()) == this->resMap.end())
	
	//add the resource to the resources map of ResMan
	auto result = this->resMap.insert(
			std::pair<const char*, ting::WeakRef<Resource> >(
					node.Value(),
					res.GetWeakRef()
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



Resource::~Resource()noexcept{
	morda::App::Inst().resMan.resMap.erase(this->resMapIter);
}

