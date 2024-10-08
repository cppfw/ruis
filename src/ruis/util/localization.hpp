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

#include <map>
#include <string>

#include <tml/tree.hpp>
#include <utki/debug.hpp>
#include <utki/shared_ref.hpp>

namespace ruis {

class wording
{
	friend class localization;

	using vocabulary_type = std::map<std::string, std::u32string, std::less<>>;

	std::shared_ptr<vocabulary_type> vocabulary;

	vocabulary_type::const_iterator iter;

	wording(
		std::shared_ptr<vocabulary_type> vocabulary, //
		std::string_view id
	);

public:
	wording() = default;

	bool empty() const noexcept
	{
		return !this->vocabulary;
	}

	const std::string& id() const
	{
		ASSERT(!this->empty())
		return this->iter->first;
	}

	const std::u32string& string() const
	{
		ASSERT(!this->empty())
		return this->iter->second;
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
