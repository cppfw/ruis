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

#include "layout_factory.hpp"

using namespace morda;

void layout_factory::add_factory(std::string name, factory_type factory)
{
	this->factories.emplace(std::move(name), std::move(factory));
}

utki::shared_ref<layout> layout_factory::create(std::string_view name, const tml::forest& desc)
{
	auto i = this->factories.find(name);
	if (i == this->factories.end()) {
		std::stringstream ss;
		ss << "layout_factory::create(" << name << "): layout factory not found";
		throw std::logic_error(ss.str());
	}

	return i->second(desc);
}
