/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "../base/image_button.hpp"
#include "../toggle_button.hpp"

namespace ruis {

class image_toggle :
	virtual public toggle_button, //
	public image_button
{
public:
	image_toggle(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	image_toggle(const image_toggle&) = delete;
	image_toggle& operator=(const image_toggle&) = delete;

	image_toggle(image_toggle&&) = delete;
	image_toggle& operator=(image_toggle&&) = delete;

	~image_toggle() override = default;
};

} // namespace ruis
