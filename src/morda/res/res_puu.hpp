#pragma once

#include "../resource_loader.hpp"

#include <puu/tree.hpp>

#include "../util/util.hpp"

namespace morda{

/**
 * @brief Resource holding a puu forest.
 * 
 * %Resource description:
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
class res_puu : public Resource{
	friend class resource_loader;
	
	puu::forest s;
public:
	/**
	 * @brief Create puu resource.
	 * @param s - puu forest to initialize the resource with.
	 */
	res_puu(puu::forest&& s);
	
	res_puu(const res_puu&) = delete;
	res_puu& operator=(const res_puu&) = delete;
	
	const puu::forest& forest()const noexcept{
		return this->s;
	}
	
private:
	static std::shared_ptr<res_puu> load(const puu::forest& desc, const papki::file& fi);
};

}
