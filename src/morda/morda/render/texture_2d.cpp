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

#include "texture_2d.hpp"

using namespace morda;

unsigned texture_2d::bytes_per_pixel(texture_2d::type t){
	switch(t){
		case texture_2d::type::grey:
			return 1;
		case texture_2d::type::grey_alpha:
			return 2;
		case texture_2d::type::rgb:
			return 3;
		case texture_2d::type::rgba:
			return 4;
		default:
			return 0;
	}
}
