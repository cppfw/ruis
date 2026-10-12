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

namespace ruis {

/**
 * @brief Kind of the keyboard to use for the text input.
 * The values correspond to the keyboard layouts of the Android
 * `android.text.InputType` class; on other platforms a
 * platform-appropriate closest match is used. The other Android text
 * variations (EMAIL_SUBJECT, SHORT_MESSAGE, LONG_MESSAGE, PERSON_NAME,
 * POSTAL_ADDRESS, FILTER, VISIBLE_PASSWORD and the "web form"
 * variations) are not exposed, because they produce the same
 * keyboards as the values listed here.
 */
enum class keyboard_kind {
	/**
	 * @brief General (alphanumeric) keyboard, with word suggestions.
	 */
	general,

	/**
	 * @brief General (alphanumeric) keyboard, without word suggestions.
	 */
	general_no_suggestions,

	/**
	 * @brief URI keyboard (additional keys for entering URIs).
	 */
	uri,

	/**
	 * @brief E-mail address keyboard (additional '@' and '.' keys).
	 */
	email,

	/**
	 * @brief Password keyboard, the entered characters are hidden.
	 */
	password,

	/**
	 * @brief Keyboard for entering phonetic pronunciation text,
	 * such as a phonetic name field in contacts.
	 */
	phonetic,

	/**
	 * @brief Numeric keyboard (digits only).
	 */
	numeric,

	/**
	 * @brief Numeric keyboard with a '-' key for negative numbers.
	 */
	signed_numeric,

	/**
	 * @brief Numeric keyboard with a decimal separator key.
	 */
	decimal_numeric,

	/**
	 * @brief Numeric keyboard with a '-' and a decimal separator key.
	 */
	signed_decimal_numeric,

	/**
	 * @brief Phone number keyboard (digits, '*', '#', '+').
	 */
	phone_number,

	/**
	 * @brief Date and time keyboard.
	 */
	date_time
};

} // namespace ruis
