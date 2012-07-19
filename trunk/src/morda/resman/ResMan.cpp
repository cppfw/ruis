#include "ResMan.hpp"


const char* DResTag = "res";
const char* DIncludeTag = "include";



//Clone one xml note to another
static void CloneXMLNodeToXMLNode(pugi::xml_node& dest, const pugi::xml_node& source){
	assert(source.type() != pugi::node_null && source.type() != pugi::node_document && source.type() == dest.type());

	switch (source.type()){
		case pugi::node_pcdata:
		case pugi::node_cdata:
		case pugi::node_comment:
		case pugi::node_pi:
			dest.set_value(source.value());
			break;

		case pugi::node_element:
			dest.set_name(source.name());

			// copy attributes
			for(pugi::xml_attribute attr = source.first_attribute(); attr; attr = attr.next_attribute()){
				dest.append_attribute(attr.name()) = attr.value();
			}

			// recursively copy children
			for(pugi::xml_node child = source.first_child(); child; child = child.next_sibling()){
				pugi::xml_node new_child = dest.append_child(child.type());
				CloneXMLNodeToXMLNode(new_child, child);
			}

			break;

		default:
			assert(!"unexpected node type");
			break;
	}
}



//Returns true on success, false otherwise
static bool LoadXMLDocument(pugi::xml_document &doc, File& fi){
	ting::Array<ting::u8> rawFile;
	try{
		rawFile = fi.LoadWholeFileIntoMemory();
	}catch(File::Exc& e){
		return false;
	}

	if(!doc.load_buffer(rawFile.Begin(), rawFile.Size())){
		return false;
	}
	
	return true;
}



static void AppendIncludes(pugi::xml_document &doc, File& fi){
//	TRACE(<< "AppendIncludes(): enter" << std::endl)

	pugi::xml_node inc = doc.child(DIncludeTag);
	while(!inc.empty()){
		pugi::xml_attribute fileAttr = inc.attribute("file");
		if(fileAttr.empty()){
			TRACE_AND_LOG(<< "ResMan::AppendInclude(): no \"file\" attribute in include tag" << std::endl)
			break;
		}
		const char* fileName = fileAttr.value();
		ASSERT(fileName)
//		TRACE(<< "AppendIncludes(): inc file = " << fileName << std::endl)

		//Load the document to include
		pugi::xml_document incDoc;

		ASSERT(!fi.IsOpened())
		fi.SetPath(fileName);
		if(!LoadXMLDocument(incDoc, fi)){
			TRACE_AND_LOG(<< "ResMan::AppendInclude(): unable to read file" << std::endl)
			break;
		}

		if(incDoc.child(DResDescrTag).empty()){
			TRACE_AND_LOG(<< "ResMan::AppendInclude(): no ResourcesDescription signature found, file = " << fileName << std::endl)
			break;
		}

		AppendIncludes(incDoc, fi);//recursive call

		//append all entries which go after <ResourceDescription/> tag to doc
		pugi::xml_node r = incDoc.child(DResDescrTag).next_sibling();
		while(!r.empty()){
			pugi::xml_node newNode = doc.insert_child_after(r.type(), inc);
			ASSERT(!newNode.empty())
			CloneXMLNodeToXMLNode(newNode, r);
			r = r.next_sibling();
		}

		pugi::xml_node elToRemove = inc;
		inc = inc.next_sibling(DIncludeTag);
		doc.remove_child(elToRemove);
	}//~while(include tag)
//	TRACE(<< "AppendIncludes(): exit" << std::endl)
}



void ResMan::MountResPack(ting::Ptr<File> fi){
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



void ResMan::AddResource(Ref<Resource> res, const std::string& resName){
	ASS(res)->rm = this->resMap; //save weak reference to resource map

	//add the resource to the resources map of ResMan
	typedef std::pair<T_ResMapIter, bool> T_RetPair;
	typedef std::pair<std::string, WeakRef<Resource> > T_Pair;
	T_RetPair pr = this->resMap->rm.insert(T_Pair(resName, WeakRef<Resource>(res)));
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