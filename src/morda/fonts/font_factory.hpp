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

#include <utki/shared_ref.hpp>

#include "font.hpp"

namespace morda {

class font_factory
{
protected:
	const utki::shared_ref<morda::context> context;

public:
	virtual utki::shared_ref<font> create(size_t size) = 0;

	font_factory(const utki::shared_ref<morda::context>& context) :
		context(context)
	{}

	font_factory(const font_factory&) = delete;
	font_factory& operator=(const font_factory&) = delete;

	font_factory(font_factory&&) = delete;
	font_factory& operator=(font_factory&&) = delete;

	virtual ~font_factory() = default;
};

} // namespace morda
