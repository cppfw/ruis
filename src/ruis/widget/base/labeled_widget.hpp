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

#include "../label/text.hpp"

namespace ruis {

/**
 * @brief A widget which displays a text label.
 */
class labeled_widget : virtual public widget
{
	text& label;

public:
	/**
	 * @brief Labeled widget parameters.
	 */
	struct parameters {
		/**
		 * @brief Parameters of the text label widget.
		 */
		ruis::text::parameters text;

		/**
		 * @brief Label text.
		 */
		ruis::string string;
	};

protected:
	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param label - text label widget.
	 */
	labeled_widget(
		const utki::shared_ref<ruis::context>& context, //
		text& label
	) :
		widget(context, {}, {}),
		label(label)
	{}

public:
	/**
	 * @brief Get the label widget.
	 * @return the text label widget.
	 */
	text& get_label()
	{
		return this->label;
	}
};

} // namespace ruis
