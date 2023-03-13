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

#include "../../container.hpp"

namespace morda{

class size_container : public container{
public:
	size_container(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);

	size_container(const size_container&) = delete;
	size_container& operator=(const size_container&) = delete;

	vector2 measure(const vector2& quotum)const override;

	void lay_out()override;
};

}
