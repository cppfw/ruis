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

#include <map>

#include <utki/shared_ref.hpp>

#include "font.hpp"

namespace morda {

class font_provider
{
	mutable std::map<real, std::weak_ptr<const font>> cache;

protected:
	const utki::shared_ref<morda::context> context;

	virtual utki::shared_ref<const font> create(real size) const = 0;

public:
	utki::shared_ref<const font> get(real size) const;

	// NOLINTNEXTLINE(modernize-pass-by-value)
	font_provider(const utki::shared_ref<morda::context>& context) :
		context(context)
	{}

	font_provider(const font_provider&) = delete;
	font_provider& operator=(const font_provider&) = delete;

	font_provider(font_provider&&) = delete;
	font_provider& operator=(font_provider&&) = delete;

	virtual ~font_provider() = default;
};

} // namespace morda
