/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

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

	std::map<const std::string, std::weak_ptr<resource>> res_map;

	class res_pack_entry{
	public:
		res_pack_entry() = default;

		// For MSVC compiler, it does not generate move constructor automatically
		// TODO: check if this is still needed.
		res_pack_entry(res_pack_entry&& r){
			this->fi = std::move(r.fi);
			this->script = std::move(r.script);
		}

		std::unique_ptr<const papki::file> fi;
		treeml::forest script;
	};

	std::vector<res_pack_entry> resPacks;

	class find_in_script_result{
	public:
		find_in_script_result(res_pack_entry& resPack, const treeml::tree& element) :
				rp(resPack),
				e(element)
		{}

		res_pack_entry& rp;
		const treeml::tree& e;
	};

	find_in_script_result find_resource_in_script(const std::string& resName);

	template <class T> std::shared_ptr<T> find_resource_in_res_map(const char* resName);

	// add resource to resources map
	void add_resource(const utki::shared_ref<resource>& res, const std::string& name);

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
	 * @throw TODO:
	 */
	template <class T> utki::shared_ref<T> load(const char* name);

	template <class T> utki::shared_ref<T> load(const std::string& name){
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
	const utki::shared_ref<morda::context> context;

	// this can only be used as a base class
	resource(const utki::shared_ref<morda::context>& c) :
			context(c)
	{}
public:
	virtual ~resource()noexcept{}
};

template <class T> std::shared_ptr<T> resource_loader::find_resource_in_res_map(const char* resName){
	auto i = this->res_map.find(resName);
	if(i != this->res_map.end()){
		if(auto r = (*i).second.lock()){
			return std::dynamic_pointer_cast<T>(std::move(r));
		}
		this->res_map.erase(i);
	}
	return nullptr; // no resource found with given name, return invalid reference
}

template <class T> utki::shared_ref<T> resource_loader::load(const char* resName){
//	TRACE(<< "ResMan::Load(): enter" << std::endl)
	if(auto r = this->find_resource_in_res_map<T>(resName)){
//		TRACE(<< "ResManHGE::Load(): resource found in map" << std::endl)
		return utki::shared_ref<T>(std::move(r));
	}

//	TRACE(<< "ResMan::Load(): searching for resource in script..." << std::endl)
	find_in_script_result ret = this->find_resource_in_script(resName);
	ASSERT(ret.rp.fi)

//	TRACE(<< "ResMan::Load(): resource found in script" << std::endl)

	auto resource = T::load(this->ctx, ret.e.children, *ret.rp.fi);

	this->add_resource(resource, ret.e.value.to_string());

//	TRACE(<< "ResMan::LoadTexture(): exit" << std::endl)
	return resource;
}

}
