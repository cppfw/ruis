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

	ASS(rpe.fi)->SetPath("main.res");
	
	ting::Ptr<stob::Node> resScript = stob::Load(*(rpe.fi));

	//handle includes
	ResolveIncludes(*(rpe.fi), resScript.operator->());
	
	rpe.resScript = resScript;

	this->resPacks.push_back(rpe);
}



ResourceManager::FindInScriptRet ResourceManager::FindResourceInScript(const std::string& resName){
//	TRACE(<< "ResMan::FindResourceInScript(): resName = " << (resName.c_str()) << std::endl)

	for(T_ResPackIter i = this->resPacks.begin(); i != this->resPacks.end(); ++i){
		for(const stob::Node* e = (*i).resScript->Child(DResTag).second; e; e = e->Next(DResTag).second){
			const stob::Node* nameNode = e->Child("name").second;
			if(!nameNode){
				TRACE(<< "ResMan::FindResourceInScript(): WARNING! no 'name' property in resource" << std::endl)
				continue;
			}
			const stob::Node* name = nameNode->Child();
			if(!name){
				TRACE(<< "ResMan::FindResourceInScript(): WARNING! 'name' property in resource has no value" << std::endl)
				continue;
			}
//			TRACE(<< "ResMan::FindResourceInScript(): name = " << name << std::endl)
			if(resName.compare(name->Value()) == 0){
//				TRACE(<< "ResMan::FindResourceInScript(): resource found" << std::endl)
				return FindInScriptRet(&(*i), e);
			}
		}//~for(res)
	}//~for(resPack)
	throw ResourceManager::Exc("resource name not found in mounted resource packs");
}



void ResourceManager::AddResource(const ting::Ref<Resource>& res, const stob::Node* node){
	ASSERT(res)
	
	//add the resource to the resources map of ResMan
	std::pair<T_ResMapIter, bool> pr = this->resMap->rm.insert(
			std::pair<const std::string*, ting::WeakRef<Resource> >(
					&node->Value(),
					res.GetWeakRef()
				)
		);
	
	ASSERT(pr.second == true) //make sure that the new element was added but not the old one rewritten

	res->it = pr.first;
	res->rm = this->resMap; //save weak reference to resource map
}
