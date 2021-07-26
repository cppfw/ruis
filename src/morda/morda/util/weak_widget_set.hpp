/*
morda GUI framework
Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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

#include "../widgets/widget.hpp"

#include <set>

namespace morda{

class weak_widget_set{
	typedef std::weak_ptr<widget> pointer_type;
	std::set<pointer_type, std::owner_less<pointer_type>> set;
public:
	weak_widget_set(){}

	weak_widget_set(decltype(set)&& set) :
			set(std::move(set))
	{}

	void add(decltype(set)::value_type w);

	void foreach(const std::function<void(morda::widget&)>& do_this);

	void set_enabled(bool enabled);
};

}
