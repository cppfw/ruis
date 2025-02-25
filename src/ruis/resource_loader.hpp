/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include <list>
#include <map>

#include <papki/file.hpp>
#include <tml/tree.hpp>
#include <utki/shared.hpp>
#include <utki/util.hpp>

namespace ruis {

class resource;
class context;

/**
 * @brief Resource loader.
 * This class manages application recources loading from tml resource description scripts.
 *
 * Format of resource description scripts is simple. It uses tml markup.
 * Each resource is a root-level tml node, the value is a name of the resource, by that name
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
class resource_loader
{
	friend class context;
	friend class resource;

	class res_pack_entry
	{
	public:
		res_pack_entry(res_pack_entry&& r) = delete;
		res_pack_entry& operator=(res_pack_entry&&) = delete;

		res_pack_entry(const res_pack_entry&) = delete;
		res_pack_entry& operator=(const res_pack_entry&) = delete;

		std::unique_ptr<const papki::file> fi;

		// TODO: use std::map/std::unordered_map?
		tml::forest script;

		res_pack_entry(decltype(fi) fi, decltype(script) script) :
			fi(std::move(fi)),
			script(std::move(script))
		{}

		~res_pack_entry() = default;

		// keep loaded resources map in res_pack_entry, so that unmounted
		// resource pack removes it's loaded resources map
		// TODO: use std::unordered_map?
		std::map<const std::string, std::weak_ptr<resource>, std::less<>> res_map;

		void add_resource_to_res_map(const utki::shared_ref<resource>& res, std::string_view id);
		std::shared_ptr<resource> find_resource_in_res_map(std::string_view id);
		const tml::forest* find_resource_in_script(std::string_view id) const;
	};

	// use std::list to be able to use iterator as resource pack id
	std::list<res_pack_entry> res_packs;

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
	 * @param fi - file interface pointing to the resource pack's description script.
	 *             If file interface points to a directory instead of a file then
	 *             resource description filename is assumed to be "main.res".
	 * @return mounted resource pack id.
	 */
	decltype(res_packs)::const_iterator mount_res_pack(const papki::file& fi);

	/**
	 * @brief Unmount mounted resource pack.
	 * @param id - id of the resource pack to unmount.
	 */
	void unmount_res_pack(decltype(res_packs)::const_iterator id);

	/**
	 * @brief Load a resource.
	 * This is a template function. Resource types are not indicated anyhow in
	 * resource descriptions, so it is necessary to indicate resource of which type
	 * you are loading by specifying a resource class as a template parameter of the function.
	 *
	 * Example:
	 * @code
	 * auto image = this->context.get().loader().load<ruis::res_image>("img_my_image_name");
	 * @endcode
	 *
	 * @param id - id of the resource as it appears in resource description.
	 * @return Loaded resource.
	 * @throw TODO:
	 */
	template <class resource_type>
	utki::shared_ref<resource_type> load(std::string_view id);

private:
};

/**
 * @brief Base class for all resources.
 */
class resource : virtual public utki::shared
{
	friend class resource_loader;

	std::string id;

protected:
	const utki::shared_ref<ruis::context> context;

	// NOLINTNEXTLINE(modernize-pass-by-value)
	resource(utki::shared_ref<ruis::context> c) :
		context(std::move(c))
	{}

public:
	resource(const resource&) = delete;
	resource& operator=(const resource&) = delete;

	resource(resource&&) = delete;
	resource& operator=(resource&&) = delete;

	~resource() override = default;

	std::string_view get_id() const
	{
		return this->id;
	}
};

template <class resource_type>
utki::shared_ref<resource_type> resource_loader::load(std::string_view id)
{
	for (auto& res_pack : utki::views::reverse(this->res_packs)) {
		if (auto r = res_pack.find_resource_in_res_map(id)) {
			return utki::shared_ref<resource_type>(std::dynamic_pointer_cast<resource_type>(r));
		}

		auto desc = res_pack.find_resource_in_script(id);
		if (!desc) {
			continue;
		}

		try {
			ASSERT(res_pack.fi)
			auto resource = resource_type::load(utki::make_shared_from(this->ctx), *desc, *res_pack.fi);

			// resource need to know its id so that it would be possible to reload the resource
			// using the id in case mounted resource packs change
			resource.get().id = id;

			res_pack.add_resource_to_res_map(resource, id);

			return resource;
		} catch (...) {
			utki::log([&](auto& o) {
				o << "could not load resource, id = " << id << std::endl;
			});
			throw;
		}
	}

	LOG([&](auto& o) {
		o << "resource id not found in mounted resource packs: " << id << std::endl;
	})
	std::stringstream ss;
	ss << "resource id not found in mounted resource packs: " << id;
	throw std::logic_error(ss.str());
}

} // namespace ruis
