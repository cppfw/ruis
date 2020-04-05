#pragma once

#include "../resource_loader.hpp"

#include <puu/tree.hpp>

#include "../util/util.hpp"

namespace morda{

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
class res_puu : public resource{
	friend class resource_loader;
	
	puu::forest s;
public:
	/**
	 * @brief Create puu resource.
	 * @param c - context.
	 * @param s - puu forest to initialize the resource with.
	 */
	res_puu(std::shared_ptr<morda::context> c, puu::forest&& s);
	
	res_puu(const res_puu&) = delete;
	res_puu& operator=(const res_puu&) = delete;
	
	const puu::forest& forest()const noexcept{
		return this->s;
	}
	
private:
	static std::shared_ptr<res_puu> load(morda::context& ctx, const puu::forest& desc, const papki::file& fi);
};

}
