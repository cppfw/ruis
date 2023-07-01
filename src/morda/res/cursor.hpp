/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "image.hpp"

namespace morda::res {

/**
 * @brief Mouse cursor resource.
 *
 * @param image - reference to image resource.
 * @param hotspot - X and Y of the hot spot from top left corner of the image.
 *
 * Example:
 * @code
 * crs_arrow{
 *     image{img_arrow}
 *     hotspot{2 3}
 * }
 * @endcode
 */
class cursor : public resource
{
	utki::shared_ref<const res::image> image_v;
	vector2 hotspot_v;

public:
	cursor(
		const utki::shared_ref<morda::context>& c,
		const utki::shared_ref<morda::res::image>& image,
		const vector2& hotspot
	);

	cursor(const cursor&) = delete;
	cursor& operator=(const cursor&) = delete;

	cursor(cursor&&) = delete;
	cursor& operator=(cursor&&) = delete;

	~cursor() override = default;

	const res::image& image() const noexcept
	{
		return this->image_v.get();
	}

	const vector2& hotspot() const noexcept
	{
		return this->hotspot_v;
	}

private:
	friend class morda::resource_loader;

	static utki::shared_ref<cursor> load(
		const utki::shared_ref<morda::context>& ctx,
		const ::treeml::forest& desc,
		const papki::file& fi
	);
};

} // namespace morda::res
