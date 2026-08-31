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

#include "../../../util/color.hpp"
#include "../../../util/content_wrapping.hpp"
#include "../../../util/length.hpp"

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
	public content_wrapping,
	private container
{
public:
	struct parameters {
		/**
		 * @brief Color of the dimming background behind the dialog.
		 * Defaults to the 'color_dimmed' style value if undefined.
		 */
		// TODO: remove for now
		styled<color> dim_color;

		/**
		 * @brief Color of the dialog panel background.
		 * Defaults to the 'color_panel' style value if undefined.
		 */
		// TODO: should be color_params
		styled<color> panel_color;

		/**
		 * @brief Margin between the dialog panel and the edge of the dialog area.
		 * Defaults to the 'len_dialog_margin' style value if undefined.
		 */
		styled<length> margin;

		/**
		 * @brief Padding between the dialog panel background and the dialog content.
		 * Defaults to the 'len_dialog_padding' style value if undefined.
		 */
		// TODO: should be padding_params
		styled<length> padding;

		/**
		 * @brief Corner radii of the dialog panel background.
		 * Defaults to the 'len_dialog_padding' style value if undefined.
		 */
		// TODO: should it be a rectangle_params?
		styled<length> corner_radius;
	};

	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		parameters dialog_params;
	};

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
 * Default content layout is column.
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
