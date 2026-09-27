/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include <map>

#include <utki/shared_ref.hpp>

#include "font.hpp"

namespace ruis {

class font_face
{
	mutable std::map<real, std::weak_ptr<const font>> cache;

protected:
	virtual utki::shared_ref<const font> create(real size) const = 0;

public:
	utki::shared_ref<const font> get(real size) const;

	font_face() = default;

	font_face(const font_face&) = delete;
	font_face& operator=(const font_face&) = delete;

	font_face(font_face&&) = delete;
	font_face& operator=(font_face&&) = delete;

	virtual ~font_face() = default;
};

} // namespace ruis
