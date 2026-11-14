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

#include <utki/enum_array.hpp>

#include "../../res/font.hpp"
#include "../widget.hpp"

namespace ruis {
/**
 * @brief A widget which renders text using a font.
 */
class font_widget : virtual public widget
{
public:
	struct parameters {
		/**
		 * @brief Font size.
		 * The default font size is a 'font_size_primary' style value.
		 */
		styled<length> size;

		/**
		 * @brief Font face.
		 * The default font face is a 'font_face_primary' style value.
		 */
		styled<res::font> face;
	};

private:
	parameters params;

	utki::enum_array<
		std::shared_ptr<const ruis::font>, //
		res::font::style //
		>
		fonts;

	/**
	 * @brief Update fonts for the currently set font face.
	 */
	void update_fonts();

	/**
	 * @brief Update fonts for the currently set font face and notify about the change.
	 */
	void update_fonts_and_notify();

protected:
	/**
	 * @brief Constructor.
	 * @param context - ruis context.
	 * @param params - font widget parameters.
	 */
	font_widget(
		const utki::shared_ref<ruis::context>& context, //
		parameters params
	);

	/**
	 * @brief Set font face.
	 * @param face - font face to set.
	 */
	void set_font_face(styled<res::font> face);

	/**
	 * @brief Set font size.
	 * @param size - font size to set.
	 */
	void set_font_size(styled<length> size);

	/**
	 * @brief Get current font size.
	 * @return current font size.
	 */
	const length& get_font_size() const noexcept
	{
		return this->params.size.get();
	}

	/**
	 * @brief Get font for drawing.
	 * Gets font of the specified style.
	 * @return Font for drawing.
	 * @throw std::logic_error in case there is no font face currently set.
	 */
	const ruis::font& get_font(res::font::style style = res::font::style::normal) const;

	/**
	 * @brief Invoked when font parameters (face or size) have changed.
	 */
	virtual void on_font_change() {}

	/**
	 * @brief Invoked when the widget is requested to reload its fonts.
	 */
	void on_reload() override;
};
} // namespace ruis