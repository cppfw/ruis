#pragma once

#include <map>

#include <utki/shared.hpp>
#include <papki/file.hpp>
#include <treeml/tree.hpp>



namespace morda{



class resource;
class context;


/**
 * @brief Resource loader.
 * This class manages application recources loading from treeml resource description scripts.
 *
 * Format of resource description scripts is simple. It uses treeml markup.
 * Each resource is a root-level treeml node, the value is a name of the resource, by that name
 * the application will load that resource.The children of resource name are the properties of the resource.
 * Each resource type defines their own properties.
 *
 * It is also possible to include another resource descriptions using the include directive at the root level.
 *
 * Example:
 * @code
 * // include another resource script into this one
 * include{some_other.res}
 *
 * // Example of resource declaration
 * img_may_image_resource // resource name
 * {
 *     // image resource has only one attribute 'file' which tells from
 *     // from which file to load the image
 *     file{sample_image.png}
 * }
 *
 * @endcode
 */
class resource_loader{
	friend class context;
	friend class resource;

	std::map<const std::string, std::weak_ptr<resource>> resMap;

	class ResPackEntry{
	public:
		ResPackEntry() = default;

		// For MSVC compiler, it does not generate move constructor automatically
		// TODO: check if this is still needed.
		ResPackEntry(ResPackEntry&& r){
			this->fi = std::move(r.fi);
			this->script = std::move(r.script);
		}

		std::unique_ptr<const papki::file> fi;
		treeml::forest script;
	};

	std::vector<ResPackEntry> resPacks;

	class FindInScriptRet{
	public:
		FindInScriptRet(ResPackEntry& resPack, const treeml::tree& element) :
				rp(resPack),
				e(element)
		{}

		ResPackEntry& rp;
		const treeml::tree& e;
	};

	FindInScriptRet findResourceInScript(const std::string& resName);

	template <class T> std::shared_ptr<T> findResourceInResMap(const char* resName);

	// Add resource to resources map
	void addResource(const std::shared_ptr<resource>& res, const std::string& name);

private:
	context& ctx;
	resource_loader(context& ctx) :
			ctx(ctx)
	{}

public:
	/**
	 * @brief Mount a resource pack.
	 * This function adds a resource pack to the list of known resource packs.
	 * It loads the resource description and uses it when searching for resource
	 * when resource loading is needed.
	 * @param fi - file interface pointing to the resource pack's STOB description.
	 *             If file interface points to a directory instead of a file then
	 *             resource description filename is assumed to be "main.res.stob".
	 */
	void mount_res_pack(const papki::file& fi);

	/**
	 * @brief Load a resource.
	 * This is a template function. Resource types are not indicated anyhow in
	 * resource descriptions, so it is necessary to indicate resource of which type
	 * you are loading by specifying a resource class as a template parameter of the function.
	 *
	 * Example:
	 * @code
	 * auto image = this->context->loader().load<morda::res_image>("img_my_image_name");
	 * @endcode
	 *
	 * @param name - name of the resource as it appears in resource description.
	 * @return Loaded resource.
	 */
	template <class T> std::shared_ptr<T> load(const char* name);

	template <class T> std::shared_ptr<T> load(const std::string& name){
		return this->load<T>(name.c_str());
	}

private:
};



/**
 * @brief Base class for all resources.
 */
class resource : virtual public utki::shared{
	friend class resource_loader;
protected:
	const std::shared_ptr<morda::context> context;

	// this can only be used as a base class
	resource(std::shared_ptr<morda::context> c) :
			context(std::move(c))
	{
		if(!this->context){
			throw std::invalid_argument("res_image::res_image(): passed in context is null");
		}
	}
public:
	virtual ~resource()noexcept{}
};



template <class T> std::shared_ptr<T> resource_loader::findResourceInResMap(const char* resName){
	auto i = this->resMap.find(resName);
	if(i != this->resMap.end()){
		if(auto r = (*i).second.lock()){
			return std::dynamic_pointer_cast<T>(std::move(r));
		}
		this->resMap.erase(i);
	}
	return nullptr;//no resource found with given name, return invalid reference
}



template <class T> std::shared_ptr<T> resource_loader::load(const char* resName){
//	TRACE(<< "ResMan::Load(): enter" << std::endl)
	if(auto r = this->findResourceInResMap<T>(resName)){
//		TRACE(<< "ResManHGE::Load(): resource found in map" << std::endl)
		return r;
	}

//	TRACE(<< "ResMan::Load(): searching for resource in script..." << std::endl)
	FindInScriptRet ret = this->findResourceInScript(resName);
	ASSERT(ret.rp.fi)

//	TRACE(<< "ResMan::Load(): resource found in script" << std::endl)

	auto resource = T::load(this->ctx, ret.e.children, *ret.rp.fi);

	this->addResource(resource, ret.e.value.to_string());

//	TRACE(<< "ResMan::LoadTexture(): exit" << std::endl)
	return resource;
}

}
