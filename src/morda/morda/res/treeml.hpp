#pragma once

#include "../resource_loader.hpp"

#include <treeml/tree.hpp>

#include "../util/util.hpp"

namespace morda{ namespace res{

/**
 * @brief resource holding a treeml forest.
 * 
 * %resource description:
 * 
 * @param file - file to read the treeml from.
 * 
 * Example:
 * @code
 * stb_sample_treeml{
 *     file{sample.tml}
 * }
 * @endcode
 */
class treeml : public resource{
	friend class morda::resource_loader;
	
	::treeml::forest s;
public:
	/**
	 * @brief Create treeml resource.
	 * @param c - context.
	 * @param s - treeml forest to initialize the resource with.
	 */
	treeml(std::shared_ptr<morda::context> c, ::treeml::forest&& s);
	
	treeml(const treeml&) = delete;
	treeml& operator=(const treeml&) = delete;
	
	const ::treeml::forest& forest()const noexcept{
		return this->s;
	}
	
private:
	static std::shared_ptr<treeml> load(morda::context& ctx, const ::treeml::forest& desc, const papki::file& fi);
};

}}
