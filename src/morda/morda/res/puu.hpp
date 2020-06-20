#pragma once

#include "../resource_loader.hpp"

#include <puu/tree.hpp>

#include "../util/util.hpp"

namespace morda{ namespace res{

/**
 * @brief resource holding a puu forest.
 * 
 * %resource description:
 * 
 * @param file - file to read the puu from.
 * 
 * Example:
 * @code
 * stb_sample_puu{
 *     file{sample.puu}
 * }
 * @endcode
 */
class puu : public resource{
	friend class morda::resource_loader;
	
	::puu::forest s;
public:
	/**
	 * @brief Create puu resource.
	 * @param c - context.
	 * @param s - puu forest to initialize the resource with.
	 */
	puu(std::shared_ptr<morda::context> c, ::puu::forest&& s);
	
	puu(const puu&) = delete;
	puu& operator=(const puu&) = delete;
	
	const ::puu::forest& forest()const noexcept{
		return this->s;
	}
	
private:
	static std::shared_ptr<puu> load(morda::context& ctx, const ::puu::forest& desc, const papki::file& fi);
};

}}
