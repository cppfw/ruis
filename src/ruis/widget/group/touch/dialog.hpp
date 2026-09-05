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

#include "../../base/color_widget.hpp"
#include "../../base/decorated_widget.hpp"
#include "../../label/padding.hpp"
#include "../../label/rectangle.hpp"

namespace ruis::touch {

/**
 * @brief Modal dialog widget.
 * The dialog encapsulates the common (chrome) parts of a modal dialog:
 * - a dimming background covering the whole area of the dialog,
 * - a click proxy which closes the dialog when a click outside of the dialog panel occurs,
 * - a key proxy which closes the dialog when the Escape key is pressed,
 * - a styled panel background (with margin, padding, color and corner radii) containing the dialog content.
 * The dialog is meant to be shown on top of the overlay (see ruis::overlay) and by default it fills
 * the whole area it is placed into.
 * The dialog content is supplied as a widget_list to the constructor and is laid out according to the
 * supplied container parameters.
 */
// Even though container is inherited privately, the std::enable_shared_from_this base is still accessable via
// public inheritance of widget. So, the linter complaint is false-positive, suppress it.
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "false-positive")
class dialog :
	public virtual widget, //
	private container,
	public decorated_widget<container>
{
public:
	struct parameters {
		/**
		 * @brief Margin between the dialog panel and the edge of the dialog area.
		 * Defaults to the 'len_dialog_margin' style value if undefined.
		 */
		padding::parameters margin_params;
	};

	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;

		/**
		 * @brief Color of the dialog panel background.
		 * Defaults to the 'color_panel' style value if undefined.
		 */
		color_widget::parameters color_params;

		/**
		 * @brief Padding between the dialog panel background and the dialog content.
		 * Defaults to the 'len_dialog_padding' style value if undefined.
		 */
		padding::parameters padding_params;

		/**
		 * @brief Dialog panel background rectangle parameters.
		 *   - corner_radii defaults to the 'len_dialog_padding' style value if undefined.
		 *   - stroke_width defaults to 0 (filled rectangle) if undefined.
		 */
		rectangle::parameters rectangle_params;

		parameters dialog_params;
	};

private:
	dialog(
		utki::shared_ref<ruis::context>& context, //
		all_parameters& params,
		utki::shared_ref<ruis::container> content_container
	);

public:
	dialog(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

	dialog(const dialog&) = delete;
	dialog& operator=(const dialog&) = delete;

	dialog(dialog&&) = delete;
	dialog& operator=(dialog&&) = delete;

	~dialog() override = default;

	/**
	 * @brief Close the dialog.
	 * Removes the dialog widget from its parent.
	 */
	void close();
};

namespace make {
/**
 * @brief Construct 'dialog' widget.
 * Default content layout is pile.
 * @param context - ruis context.
 * @param params - 'dialog' widget parameters.
 * @param children - contents of the constructed 'dialog' widget.
 * @return newly constructed 'dialog' widget.
 */
utki::shared_ref<ruis::touch::dialog> dialog(
	utki::shared_ref<context> context, //
	dialog::all_parameters params,
	widget_list children
);
} // namespace make

} // namespace ruis::touch