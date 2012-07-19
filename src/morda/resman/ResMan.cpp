#include "ResMan.hpp"


using namespace morda;



namespace{

std::string DResTag("res");
std::string DIncludeTag("include");

}



void ResMan::MountResPack(ting::Ptr<ting::fs::File> fi){
	ASSERT(fi)
	ASSERT(!fi->IsOpened())

	ResPackEntry rpe;

	rpe.fi = fi;

	ASS(rpe.fi)->SetPath("main.res");
	
	rpe.resScript = stob::Load(*(rpe.fi));
	ASSERT(rpe.resScript)

	//handle includes
	//TODO:
//	AppendIncludes(*ASS(rpe.resScript), *ASS(rpe.fi));

	this->resPacks.push_back(rpe);
}



ResMan::FindInScriptRet ResMan::FindResourceInScript(const std::string& resName){
	TRACE(<< "ResMan::FindResourceInScript(): resName = " << (resName.c_str()) << std::endl)

	for(T_ResPackIter i = this->resPacks.begin(); i != this->resPacks.end(); ++i){
		for(stob::Node* e = (*i).resScript->Child(DResTag); e; e = e->Next(DResTag)){
			stob::Node* nameNode = e->Child("name");
			if(!nameNode){
				TRACE(<< "ResMan::FindResourceInScript(): WARNING! no 'name' property in resource" << std::endl)
				continue;
			}
			stob::Node* name = nameNode->Child();
			if(!name){
				TRACE(<< "ResMan::FindResourceInScript(): WARNING! 'name' property in resource has no value" << std::endl)
				continue;
			}
			TRACE(<< "ResMan::FindResourceInScript(): name = " << name << std::endl)
			if(resName.compare(name) == 0){
				TRACE(<< "ResMan::FindResourceInScript(): resource found" << std::endl)
				return FindInScriptRet(&(*i), e);
			}
		}//~for(res)
	}//~for(resPack)
	throw ResMan::Exc("resource name not found in mounted resource packs");
}



void ResMan::AddResource(const ting::Ref<Resource>& res, const std::string& resName){
	ASSERT(res)
	
	//add the resource to the resources map of ResMan
	std::pair<T_ResMapIter, bool> pr = this->resMap->rm.insert(
			std::pair<std::string, ting::WeakRef<Resource> >(
					resName,
					res.GetWeakRef()
				)
		);
	
	ASSERT(pr.second == true) //make sure that the new element was added but not the old one rewritten
	
	res->rm = this->resMap; //save weak reference to resource map
	res->it = pr.first;
}



Resource::~Resource()throw(){
//	TRACE(<< "Resource::~Resource(): invoked" << std::endl)
	if(ting::Ref<ResMapRC> resMap = this->rm){ //if Resource map still exists
//		TRACE(<< "Resource::~Resource(): removing resource" << std::endl)
		//NOTE: the weak ref in the map is still valid because RefCounted destructor is not
		//executed so far. It will be executed right after this destructor.
		resMap->rm.erase(this->it);
//		TRACE(<< "Resource::~Resource(): resource removed" << std::endl)
	}else{
//		TRACE(<< "Resource::~Resource(): resource map is no longer existent" << std::endl)
	}
}