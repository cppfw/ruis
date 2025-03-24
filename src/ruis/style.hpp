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

public:
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
	style_value(const tml::forest& desc) :
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

class style_sheet
{
	std::map<std::string, tml::forest, std::less<>> name_to_description_map;

public:
	style_sheet(tml::forest desc);

	const tml::forest& get(std::string_view style_name);

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
	styled<utki::shared_ref<resource_type>> get_res(std::string_view style_name)
	{
		// TODO:
	}

	template <typename resource_type>
	styled<std::shared_ptr<resource_type>> try_get_res(std::string_view style_name)
	{
		// TODO:
	}
};

} // namespace ruis
