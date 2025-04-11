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

#include "../../label/nine_patch.hpp"
#include "../push_button.hpp"

namespace ruis {

/**
 * @brief Simple push button.
 * Implementation of a push button as a nine patch which can have children.
 * In GUI scripts can be instantiated as "SimpleButton". Only available after
 * initializing standard ruis widgets.
 * Since the button is a nine_patch it can have children.
 */
class nine_patch_button :
	virtual public button, //
	public nine_patch
{
public:
	struct parameters {
		std::shared_ptr<const res::nine_patch> unpressed_nine_patch;
		std::shared_ptr<const res::nine_patch> pressed_nine_patch;
	};

private:
	parameters params;

protected:
	nine_patch_button(
		utki::shared_ref<ruis::context> context, //
		container::parameters container_params,
		button::parameters button_params,
		blending_widget::parameters blending_params,
		nine_patch::parameters nine_patch_params,
		parameters params,
		widget_list children
	);

	void on_pressed_change() override;

public:
	nine_patch_button(const nine_patch_button&) = delete;
	nine_patch_button& operator=(const nine_patch_button&) = delete;

	nine_patch_button(nine_patch_button&&) = delete;
	nine_patch_button& operator=(nine_patch_button&&) = delete;

	~nine_patch_button() override = default;

	void set_unpressed_nine_patch(std::shared_ptr<const res::nine_patch> np);

	const decltype(parameters::unpressed_nine_patch)& get_unpressed_nine_patch() const noexcept
	{
		return this->params.unpressed_nine_patch;
	}

	void set_pressed_nine_patch(std::shared_ptr<const res::nine_patch> np);

	const decltype(parameters::pressed_nine_patch)& get_pressed_nine_patch() const noexcept
	{
		return this->params.pressed_nine_patch;
	}
};

} // namespace ruis
