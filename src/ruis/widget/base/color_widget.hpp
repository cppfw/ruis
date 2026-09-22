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

#include "../../util/color.hpp"
#include "../widget.hpp"

namespace ruis {

/**
 * @brief Basic widget which has a color attribute.
 */
// TODO: doxygen
class color_widget : public virtual widget
{
private:
	styled<ruis::color> color;

protected:
	color_widget(
		const utki::shared_ref<ruis::context>& context,
		styled<ruis::color> color //
	);

public:
	color_widget(const color_widget&) = delete;
	color_widget& operator=(const color_widget&) = delete;

	color_widget(color_widget&&) = delete;
	color_widget& operator=(color_widget&&) = delete;

	~color_widget() override = default;

	void set_color(styled<ruis::color> color);

	/**
	 * @brief Get the color.
	 * @return The color.
	 */
	const ruis::color& get_color() const noexcept
	{
		return this->color.get();
	}

	/**
	 * @brief Color change callback.
	 * Invoked when color changes.
	 */
	virtual void on_color_change() {}
};

} // namespace ruis
