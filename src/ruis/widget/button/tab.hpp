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

#include "../label/nine_patch.hpp"

#include "base/choice_button.hpp"

#include "nine_patch_toggle.hpp"

namespace ruis {

class tab :
	virtual public choice_button, //
	public nine_patch_toggle
{
	bool mask_overlaps(vector2 pos);

public:
	tab(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	tab(const tab&) = delete;
	tab& operator=(const tab&) = delete;

	tab(tab&&) = delete;
	tab& operator=(tab&&) = delete;

	~tab() override = default;

protected:
	bool on_mouse_button(const mouse_button_event& event) override;

	void on_hovered_change(unsigned pointer_id) override {}

	void on_pressed_change() override;
};

} // namespace ruis
