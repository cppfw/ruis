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

#include "style.hpp"

namespace ruis {

// TODO: make a concept which requires value_type::make_from(tml::forest)
// Or value type can be derived from ruis::resource
template <typename value_type>
class styled
{
	friend class style;

public:
	constexpr static const bool is_resource = std::is_base_of_v<ruis::resource, value_type>;

	using actual_value_type = std::conditional_t<
		is_resource, //
		std::shared_ptr<const value_type>,
		value_type //
		>;

private:
	class style_value : public style::style_value_base
	{
		template <typename checked_type, typename = void>
		struct has_static_member_make_from : public std::false_type {};

		template <typename checked_type>
		struct has_static_member_make_from<
			checked_type,
			std::void_t<decltype(checked_type::make_from(tml::forest()))> //
			> : public std::true_type {};

		actual_value_type value;

		void reload(
			const tml::forest& desc, //
			const ruis::resource_loader& loader
		) override
		{
			this->value = load(desc, loader);
		}

		static actual_value_type load(
			const tml::forest& desc, //
			const ruis::resource_loader& loader
		)
		{
			if constexpr (utki::is_specialization_of_v<std::shared_ptr, actual_value_type>) {
				static_assert(
					std::is_base_of_v<ruis::resource, typename actual_value_type::element_type>,
					"shared_ptr must point to a ruis::resource"
				);
				if (desc.empty()) {
					return nullptr;
				}
				return loader.load<typename actual_value_type::element_type>(desc.front().value.string);
			} else {
				if constexpr (std::is_arithmetic_v<value_type>) {
					if (desc.empty()) {
						return 0;
					}
					return utki::string_parser(desc.front().value.string).read_number<value_type>();
				} else {
					if constexpr (has_static_member_make_from<value_type>::value) {
						return value_type::make_from(desc);
					} else {
						// use free function using ADL overload resolution
						return make_from(desc, value_type{});
					}
				}
			}
		}

	public:
		style_value(
			const tml::forest& desc, //
			const ruis::resource_loader& loader
		) :
			value(load(desc, loader))
		{}

		const actual_value_type& get_value() const noexcept
		{
			return this->value;
		}
	};

	using style_value_ref_type = utki::shared_ref<const style_value>;

	std::variant<
		style_value_ref_type, //
		actual_value_type //
		>
		value;

	styled(style_value_ref_type sv) :
		value(std::move(sv))
	{}

public:
	styled(actual_value_type value = actual_value_type()) :
		value(std::move(value))
	{}

	template <
		typename convertible_type, //
		std::enable_if_t<std::is_convertible_v<convertible_type, actual_value_type>, bool> = true //
		>
	styled(convertible_type conv) :
		value(actual_value_type(conv))
	{}

	bool is_from_style() const noexcept
	{
		return !std::holds_alternative<actual_value_type>(this->value);
	}

	const actual_value_type& get() const noexcept
	{
		// use std::get_if() to avoid throwing exceptions
		ASSERT(!this->value.valueless_by_exception())
		if (const auto* v = std::get_if<actual_value_type>(&this->value)) {
			return *v;
		}
		const auto* r = std::get_if<style_value_ref_type>(&this->value);
		ASSERT(r)
		return r->get().get_value();
	}
};

} // namespace ruis