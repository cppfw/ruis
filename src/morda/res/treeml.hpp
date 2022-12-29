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
	treeml(const utki::shared_ref<morda::context>& c, ::treeml::forest&& s);
	
	treeml(const treeml&) = delete;
	treeml& operator=(const treeml&) = delete;
	
	const ::treeml::forest& forest()const noexcept{
		return this->s;
	}
	
private:
	static std::shared_ptr<treeml> load(morda::context& ctx, const ::treeml::forest& desc, const papki::file& fi);
};

}}
