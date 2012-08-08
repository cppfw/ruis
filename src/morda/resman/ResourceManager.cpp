#include "ResourceManager.hpp"


using namespace morda;



namespace{

std::string DResTag("res");
std::string DIncludeTag("include");



//fi path should be set to resource script for resolving includes.
//return pointer to the last child node of the script
stob::Node* ResolveIncludes(ting::fs::File& fi, stob::Node* root){
	std::pair<stob::Node*, stob::Node*> n = root->Child(DIncludeTag);
	for(; n.second;){
		ASSERT(n.second)
		stob::Node* incPathNode = n.second->Child();
		if(!incPathNode){
			throw Exc("include tag without value encountered in resource script");
		}
		
		fi.SetPath(fi.ExtractDirectory() + incPathNode->Value());
		ting::Ptr<stob::Node> incNode = stob::Load(fi);
		
		//recursive call
		stob::Node* lastChild = ResolveIncludes(fi, incNode.operator->());
		
		//substitute includes
		if(!n.first){
			//include tag is the very first tag
			root->RemoveFirstChild();
			
			if(lastChild){
				ASSERT(!lastChild->Next())
				ASSERT(incNode->Child())
				lastChild->InsertNext(root->RemoveChildren());
				root->SetChildren(incNode->RemoveChildren());
				n = lastChild->Next(DIncludeTag);
			}else{
				ASSERT(!incNode->Child())
				n = n.second->Next(DIncludeTag);
			}
			continue;
		}else{
			//include tag is not the first one
			
			n.first->RemoveNext();
			if(lastChild){
				ASSERT(!lastChild->Next())
				ASSERT(incNode->Child())
				ting::Ptr<stob::Node> tail = n.first->ChopNext();
				n.first->SetNext(incNode->RemoveChildren());
				lastChild->SetNext(tail);
				n = lastChild->Next(DIncludeTag);
			}else{
				ASSERT(!incNode->Child())
				n = n.second->Next(DIncludeTag);
			}
			continue;
		}
	}
	return n.first;
}

}//~namespace



void ResourceManager::MountResPack(ting::Ptr<ting::fs::File> fi){
	ASSERT(fi)
	ASSERT(!fi->IsOpened())

	ResPackEntry rpe;

	rpe.fi = fi;

	ASS(rpe.fi)->SetPath("main.res.stob");
	
	ting::Ptr<stob::Node> resScript = stob::Load(*(rpe.fi));

	//handle includes
	ResolveIncludes(*(rpe.fi), resScript.operator->());
	
	rpe.resScript = resScript;

	this->resPacks.push_back(rpe);
	ASSERT(this->resPacks.back().fi.IsValid())
	ASSERT(this->resPacks.back().resScript.IsValid())
}



ResourceManager::FindInScriptRet ResourceManager::FindResourceInScript(const std::string& resName){
//	TRACE(<< "ResourceManager::FindResourceInScript(): resName = " << (resName.c_str()) << std::endl)

	for(T_ResPackList::iterator i = this->resPacks.begin(); i != this->resPacks.end(); ++i){
		for(const stob::Node* e = i->resScript->Child(DResTag).second; e; e = e->Next(DResTag).second){
//			TRACE(<< "ResourceManager::FindResourceInScript(): searching for 'name' property" << std::endl)
			const stob::Node* nameProp = e->GetProperty("name");
			if(!nameProp){
//				TRACE(<< "ResourceManager::FindResourceInScript(): WARNING! no 'name' property in resource" << std::endl)
				continue;
			}
//			TRACE(<< "ResourceManager::FindResourceInScript(): name = " << name << std::endl)
			if(resName.compare(nameProp->Value()) == 0){
//				TRACE(<< "ResourceManager::FindResourceInScript(): resource found" << std::endl)
				return FindInScriptRet(&(*i), e, nameProp);
			}
		}//~for(res)
	}//~for(resPack)
	TRACE(<< "resource name not found in mounted resource packs: " << resName << std::endl)
	throw ResourceManager::Exc("resource name not found in mounted resource packs");
}



void ResourceManager::AddResource(const ting::Ref<Resource>& res, const stob::Node* node){
	ASSERT(res)
	
	//add the resource to the resources map of ResMan
	std::pair<T_ResMap::iterator, bool> pr = this->resMap->rm.insert(
			std::pair<const std::string*, ting::WeakRef<Resource> >(
					&node->Value(),
					res.GetWeakRef()
				)
		);
	
	ASSERT(pr.second == true) //make sure that the new element was added but not the old one rewritten

//#ifdef DEBUG
//	for(T_ResMap::iterator i = this->resMap->rm.begin(); i != this->resMap->rm.end(); ++i){
//		TRACE(<< "\t" << *(*i).first << std::endl)
//	}
//#endif
	
	res->it = pr.first;
	res->rm = this->resMap; //save weak reference to resource map
}
