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

#include "render_factory.hpp"

using namespace morda;

utki::shared_ref<texture_2d> render_factory::create_texture_2d(
	r4::vector2<unsigned> dims,
	utki::span<const uint32_t> data
)
{
	return this->create_texture_2d(
		texture_2d::type::rgba,
		dims,
		utki::span<uint8_t>(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&*data.begin())), data.size_bytes())
	);
}
