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

#include "../group/pile.hpp"

namespace morda{

class min_proxy : public morda::pile{
	mutable std::weak_ptr<const morda::widget> target;
	mutable std::string root_id;
	mutable std::vector<std::string> target_id;

public:
	min_proxy(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	vector2 measure(const vector2& quotum)const override;
};

}
