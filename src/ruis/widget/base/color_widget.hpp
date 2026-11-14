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
class color_widget : public virtual widget
{
private:
	styled<ruis::color> color;

protected:
	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param color - initial color.
	 */
	color_widget(
		const utki::shared_ref<ruis::context>& context,
		styled<ruis::color> color //
	);

public:
	color_widget(const color_widget&) = delete;
	color_widget& operator=(const color_widget&) = delete;

	color_widget(color_widget&&) = delete;
	color_widget& operator=(color_widget&&) = delete;

	/**
	 * @brief Default destructor.
	 */
	~color_widget() override = default;

	/**
	 * @brief Set the color.
	 * @param color - the color to set.
	 */
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

	/**
	 * @brief Handle a probe (debug inspection) command.
	 *
	 * Commands handled by the color_widget class:
	 * - 'get_state' (fold): no parameters. Adds a single section named after
	 *   this class which reports the color state as the styled color: a direct
	 *   color is reported as an 'rgba' node with the 4 channel values as
	 *   children, a style-sourced color is wrapped in a 'style_ref' node.
	 *
	 * @param command - probe command.
	 * @return probe response.
	 */
	probe_response_set probe(const tml::tree& command) override;
};

} // namespace ruis
