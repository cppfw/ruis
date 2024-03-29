/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "../container.hpp"
#include "../widget.hpp"

namespace ruis {

class collapse_area :
	virtual public widget, //
	private container
{
	// TODO: naming convention
	std::shared_ptr<container> contentArea;
	std::shared_ptr<container> title_v;

public:
	collapse_area(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	container& content() noexcept
	{
		ASSERT(this->contentArea)
		return *this->contentArea;
	}

	container& title() noexcept
	{
		ASSERT(this->title_v)
		return *this->title_v;
	}
};

} // namespace ruis
