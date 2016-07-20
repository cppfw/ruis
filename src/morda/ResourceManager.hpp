#pragma once

#include <map>

#include <utki/Shared.hpp>
#include <papki/File.hpp>
#include <stob/dom.hpp>

#include "Exc.hpp"


namespace morda{



class App;
class Resource;



/**
 * @brief Resource manager.
 * This class manages application recources loading from STOB resource description scripts.
 * 
 * Format of resource description scripts is simple. It uses STOB markup.
 * Each resource is a root-level STOB node, the value is a name of the resource, by that name
 * the application will load that resource.The children of resource name are the properties of the resource.
 * Each resource type defines their own properties.
 * 
 * It is also possible to include another resource descriptions using the include directive at the root level.
 * 
 * Example:
 * @code
 * //include another resource script into this one
 * include{some_other.res.stob}
 * 
 * //Example of resource declaration
 * img_may_image_resource //resource name
 * {
 *     //image resource has only one attribute 'file' which tells from
 *     //from which file to load the image
 *     file{sample_image.png}
 * }
 * 
 * @endcode
 */
class ResourceManager{
	friend class morda::App;
	friend class Resource;
	
	std::map<const std::string, std::weak_ptr<Resource>> resMap;

	class ResPackEntry{
	public:
		ResPackEntry() = default;
		
		//For MSVC compiler, it does not generate move constructor automatically
		ResPackEntry(ResPackEntry&& r){
			this->fi = std::move(r.fi);
			this->resScript = std::move(r.resScript);
		}

		std::unique_ptr<const papki::File> fi;
		std::unique_ptr<const stob::Node> resScript;
	};

	typedef std::vector<ResPackEntry> T_ResPackList;

	//list of mounted resource packs
	T_ResPackList resPacks;


	class FindInScriptRet{
	public:
		FindInScriptRet(ResPackEntry& resPack, const stob::Node& element) :
				rp(resPack),
				e(element)
		{}

		ResPackEntry& rp;
		const stob::Node& e;
	};

	FindInScriptRet findResourceInScript(const std::string& resName);

	template <class T> std::shared_ptr<T> findResourceInResMap(const char* resName);

	//Add resource to resources map
	void addResource(const std::shared_ptr<Resource>& res, const stob::Node& node);

private:
	ResourceManager() = default;

public:
	/**
	 * @brief Basic recource related exception.
	 */
	class Exc : public morda::Exc{
	public:
		Exc(const std::string& message) :
				morda::Exc(message)
		{}
	};

	/**
	 * @brief Mount a resource pack.
	 * This function adds a resource pack to the list of known resource packs.
	 * It loads the resource description and uses it when searching for resource
	 * when resource loading is needed.
	 * @param fi - file interface pointing to the resource pack's STOB description.
	 *             If file interface points to a directory instead of a file then
	 *             resource description filename is assumed to be "main.res.stob".
	 */
	void mountResPack(const papki::File& fi);

	/**
	 * @brief Load a resource.
	 * This is a template function. Resource types are not indicated anyhow in
	 * resource descriptions, so it is necessary to indicate resource of which type
	 * you are loading by specifying a resource class as a template parameter of the function.
	 * 
	 * Example:
	 * @code
	 * auto image = morda::App::inst().resMan().load<morda::ResImage>("img_my_image_name");
	 * @endcode
	 * 
	 * @param resName - name of the resource as it appears in resource description.
	 * @return Loaded resource.
	 */
	template <class T> std::shared_ptr<T> load(const char* resName);
	
private:
};



/**
 * @brief Base class for all resources.
 */
class Resource : virtual public utki::Shared{
	friend class ResourceManager;
protected:
	//this can only be used as a base class
	Resource(){}
public:
	virtual ~Resource()noexcept{}
};



template <class T> std::shared_ptr<T> ResourceManager::findResourceInResMap(const char* resName){
	auto i = this->resMap.find(resName);
	if(i != this->resMap.end()){
		if(auto r = (*i).second.lock()){
			return std::dynamic_pointer_cast<T>(std::move(r));
		}
		this->resMap.erase(i);
	}
	return nullptr;//no resource found with given name, return invalid reference
}



template <class T> std::shared_ptr<T> ResourceManager::load(const char* resName){
//	TRACE(<< "ResMan::Load(): enter" << std::endl)
	if(auto r = this->findResourceInResMap<T>(resName)){
//		TRACE(<< "ResManHGE::Load(): resource found in map" << std::endl)
		return r;
	}

//	TRACE(<< "ResMan::Load(): searching for resource in script..." << std::endl)
	FindInScriptRet ret = this->findResourceInScript(resName);
	ASSERT(ret.rp.fi)

//	TRACE(<< "ResMan::Load(): resource found in script" << std::endl)

	if(!ret.e.child()){
		throw Exc("ResourceManager::Load(): resource description is empty");
	}
	
	auto resource = T::load(*ret.e.child(), *ret.rp.fi);

	this->addResource(resource, ret.e);

//	TRACE(<< "ResMan::LoadTexture(): exit" << std::endl)
	return resource;
}







}//~namespace
