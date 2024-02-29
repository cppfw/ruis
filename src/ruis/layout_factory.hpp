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

#include <functional>
#include <map>

#include <tml/tree.hpp>
#include <utki/shared_ref.hpp>

namespace ruis {

class layout;

class layout_factory
{
public:
	using factory_type = std::function<utki::shared_ref<layout>(const tml::forest& desc)>;

private:
	std::map<std::string, factory_type, std::less<>> factories;

public:
	void add_factory(std::string name, factory_type factory);

	utki::shared_ref<layout> create(std::string_view name, const tml::forest& desc);
};

} // namespace ruis

// include definitions for forward declared classes
#include "layout.hpp"
