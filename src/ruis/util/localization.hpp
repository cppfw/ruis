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

#include <map>
#include <string>

#include <tml/tree.hpp>
#include <utki/debug.hpp>
#include <utki/shared_ref.hpp>

#include "format.hpp"

namespace ruis {

class wording
{
	friend class localization;

	using vocabulary_type = std::map<std::string, std::u32string, std::less<>>;

	std::shared_ptr<vocabulary_type> vocabulary;
	vocabulary_type::const_iterator iter;

	std::vector<format_chunk> format_chunks;
	std::vector<std::u32string> format_args;
	std::u32string formatted_string;

	wording(
		std::shared_ptr<vocabulary_type> vocabulary, //
		std::string_view id
	);

	bool is_formatted() const noexcept
	{
		return !this->format_chunks.empty();
	}

	const std::u32string& get_string() const noexcept
	{
		return this->iter->second;
	}

public:
	wording() = default;

	bool empty() const noexcept
	{
		return !this->vocabulary;
	}

	const std::string& id() const
	{
		// TODO: make empty static string and return it in case of empty()?
		ASSERT(!this->empty())
		return this->iter->first;
	}

	/**
	 * @brief Aply formatting to this wording.
	 * @param args - replacement strings.
	 * @return Reference to this instance.
	 */
	wording& format(std::vector<std::u32string> args);

	const std::u32string& string() const noexcept
	{
		ASSERT(!this->empty())
		if (this->is_formatted()) {
			return this->formatted_string;
		} else {
			return this->get_string();
		}
	}
};

class localization
{
	utki::shared_ref<wording::vocabulary_type> vocabulary = utki::make_shared<wording::vocabulary_type>();

	static utki::shared_ref<wording::vocabulary_type> read_localization_vocabulary(const tml::forest& desc);

public:
	localization() = default;

	localization(const tml::forest& desc);

	wording get(std::string_view id);

	wording reload(wording&& w);
};

/**
 * @brief GUI string.
 * GUI string can be either exact UTF-32 string or a reference to a localized wording.
 */
using string = std::variant<
	std::u32string, //
	wording //
	>;

} // namespace ruis
