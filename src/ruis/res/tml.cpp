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

#include "tml.hpp"

#include "../context.hpp"
#include "../resource_loader.hpp"

using namespace ruis::res;

ruis::res::tml::tml(
	utki::shared_ref<ruis::context> c, //
	::tml::forest forest
) :
	resource(std::move(c)),
	s(std::move(forest))
{}

utki::shared_ref<ruis::res::tml> ruis::res::tml::load( //
	utki::shared_ref<ruis::context> ctx,
	const ::tml::forest& desc,
	const papki::file& fi
)
{
	for (auto& p : desc) {
		if (p.value == "file") {
			fi.set_path(get_property_value(p).string);
		}
	}

	return utki::make_shared<tml>( //
		std::move(ctx),
		::tml::read(fi)
	);
}
