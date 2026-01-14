/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include <tml/tree.hpp>

#include "../resource_loader.hpp"
#include "../util/util.hpp"

namespace ruis::res {

/**
 * @brief resource holding a tml forest.
 *
 * %resource description:
 *
 * @param file - file to read the tml from.
 *
 * Example:
 * @code
 * stb_sample_treeml{
 *     file{sample.tml}
 * }
 * @endcode
 */
class tml : public resource
{
	friend class ruis::resource_loader;

	::tml::forest tml_forest;

public:
	/**
	 * @brief Create tml resource.
	 * @param tml_forest - tml forest to initialize the resource with.
	 */
	tml(::tml::forest tml_forest);

	tml(const tml&) = delete;
	tml& operator=(const tml&) = delete;

	tml(tml&&) = delete;
	tml& operator=(tml&&) = delete;

	~tml() override = default;

	const ::tml::forest& forest() const noexcept
	{
		return this->tml_forest;
	}

private:
	static utki::shared_ref<tml> load( //
		const ruis::resource_loader& loader,
		const ::tml::forest& desc,
		const fsif::file& fi
	);
};

} // namespace ruis::res
