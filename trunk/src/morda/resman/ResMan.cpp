#include "ResMan.hpp"


using namespace morda;



namespace{

const char* DResTag = "res";
const char* DIncludeTag = "include";

}



void ResMan::MountResPack(ting::Ptr<ting::fs::File> fi){
	ASSERT(fi)
	ASSERT(!fi->IsOpened())

	ResPackEntry rpe;

	rpe.fi = fi;
	rpe.resScript = ting::Ptr<pugi::xml_document>(
			new pugi::xml_document()
		);

	ASS(rpe.fi)->SetPath("main.res");
	if(!LoadXMLDocument(*ASS(rpe.resScript), *ASS(rpe.fi))){
		throw ting::Exc("ResMan::MountResPack(): failed loading resource file");
	}

	if(ASS(rpe.resScript)->child("ResourcesDescription").empty()){
		throw ting::Exc("ResMan::MountResPack(): resource file does not contain <ResourcesDescription /> signature");
	}

	//handle includes
	AppendIncludes(*ASS(rpe.resScript), *ASS(rpe.fi));

	this->resPacks.push_back(rpe);
}



ResMan::FindInScriptRet ResMan::FindResourceInScript(const std::string& resName){
	M_RESOURCE_LOG(<< "ResMan::FindResourceInScript(): resName = " << (resName.c_str()) << std::endl)

	for(T_ResPackIter i = this->resPacks.begin(); i != this->resPacks.end(); ++i){
		for(pugi::xml_node e = (*i).resScript->child(DResTag); !e.empty(); e = e.next_sibling(DResTag)){
			pugi::xml_attribute nameAttr = e.attribute("name");
			if(!nameAttr.empty()){
				const char* name = nameAttr.value();
				ASSERT(name)
				M_RESOURCE_LOG(<< "ResMan::FindResourceInScript(): name = " << name << std::endl)
				if(resName.compare(name) == 0){
					M_RESOURCE_LOG(<< "ResMan::FindResourceInScript(): resource found" << std::endl)
					return FindInScriptRet(&(*i), e);
				}
			}
		}//~for(res)
	}//~for(resPack)
	return FindInScriptRet(0, pugi::xml_node());//return empty node
}



void ResMan::AddResource(const ting::Ref<Resource>& res, const std::string& resName){
	ASS(res)->rm = this->resMap; //save weak reference to resource map

	//add the resource to the resources map of ResMan
	typedef std::pair<T_ResMapIter, bool> T_RetPair;
	typedef std::pair<std::string, ting::WeakRef<Resource> > T_Pair;
	T_RetPair pr = this->resMap->rm.insert(T_Pair(resName, ting::WeakRef<Resource>(res)));
	ASSERT(pr.second == true) //make sure that the new element was added but not the old one rewritten
	res->it = pr.first;
}



Resource::~Resource()throw(){
//	TRACE(<< "Resource::~Resource(): invoked" << std::endl)
	if(Ref<ResMapRC> resMap = this->rm){ //if Resource map still exists
//		TRACE(<< "Resource::~Resource(): removing resource" << std::endl)
		//NOTE: the weak ref in the map is still valid because RefCounted destructor is not
		//executed so far. It will be executed right after this destructor.
		resMap->rm.erase(this->it);
//		TRACE(<< "Resource::~Resource(): resource removed" << std::endl)
	}else{
//		TRACE(<< "Resource::~Resource(): resource map is no longer existent" << std::endl)
	}
}