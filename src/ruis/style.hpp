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

#include <tml/tree.hpp>
#include <utki/shared_ref.hpp>

#include "resource_loader.hpp"

namespace ruis {

class style_sheet;

class style_value_base
{
protected:
	virtual void reload(const tml::forest& desc) = 0;

	style_value_base(std::string id) :
		id(std::move(id))
	{}

public:
	const std::string id;

	virtual ~style_value_base() = default;
};

template <typename value_type>
class style_value : public style_value_base
{
	value_type value;

	void reload(const tml::forest& desc) override
	{
		this->value = parse(desc);
	}

public:
	style_value(
		std::string id, //
		const tml::forest& desc
	) :
		style_value_base(std::move(id)),
		value(parse(desc))
	{}

	const value_type& get_value() const noexcept
	{
		return this->value;
	}

	static value_type parse(const tml::forest& desc);
};

template <typename value_type>
class styled
{
	using style_value_ref_type = utki::shared_ref<style_value<value_type>>;

	std::variant<
		style_value_ref_type, //
		value_type //
		>
		value;

public:
	styled(value_type value) :
		value(std::move(value))
	{}

	const value_type& get() const noexcept
	{
		return std::visit(
			[](const auto& v) -> const value_type& {
				using stored_type = utki::remove_const_reference_t<decltype(v)>;
				if constexpr (std::is_same_v<stored_type, value_type>) {
					return v;
				} else {
					static_assert(std::is_same_v<stored_type, style_value_ref_type>);
					return v.get().get_value();
				}
			},
			this->value
		);
	}
};

/**
 * @brief Style sheet.
 * TODO: write more description
 * File format:
 * @verbatim
 * version{<version-number>}
 * ruis{
 *     // ruis standard style values
 *     <style-name>{<style-value>}
 *     ...
 * }
 * user{
 *     // user defined style values
 *     <style-name>{<style-value>}
 *     ...
 * }
 * @endverbatim
 */
class style_sheet
{
	std::map<std::string, tml::forest, std::less<>> id_to_description_map;

	static std::map<std::string, tml::forest, std::less<>> parse(tml::forest desc);

public:
	style_sheet() = default;

	style_sheet(tml::forest desc);

	/**
	 * @brief Get style value description.
	 * @param style_id - id of the style value.
	 * @return pointer to the style value description if present in the style sheet.
	 * @return nullptr in case the style id is not present in the style sheet.
	 */
	const tml::forest* get(std::string_view style_id) const noexcept;

	static style_sheet load(const papki::file& fi);
};

class style
{
	utki::shared_ref<ruis::resource_loader> loader;

	std::shared_ptr<style_value_base> get(std::string_view style_name);

	// TODO: weak_ptr cache and reload cached values

public:
	style(utki::shared_ref<ruis::resource_loader> loader);

	void set(style_sheet ss);

	template <typename value_type>
	styled<value_type> get(std::string_view style_name)
	{
		// TODO:
	}

	template <typename resource_type>
	styled<std::shared_ptr<resource_type>> get_res(std::string_view style_name)
	{
		// TODO:
	}
};

} // namespace ruis
