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

class style_value_base
{
	friend class style;

protected:
	virtual void reload(
		const tml::forest& desc, //
		const ruis::resource_loader& loader
	) = 0;

	style_value_base() = default;

public:
	style_value_base(const style_value_base&) = delete;
	style_value_base& operator=(const style_value_base&) = delete;

	style_value_base(style_value_base&&) = delete;
	style_value_base& operator=(style_value_base&&) = delete;

	virtual ~style_value_base() = default;
};

// TODO: make a concept which requires value_type::default_value() and value_type::parse_value(tml::forest)
template <typename value_type>
class style_value : public style_value_base
{
	value_type value;

	void reload(
		const tml::forest& desc, //
		const ruis::resource_loader& loader
	) override
	{
		this->value = load(desc, loader);
	}

	static value_type load(
		const tml::forest& desc, //
		const ruis::resource_loader& loader
	)
	{
		if constexpr (utki::is_specialization_of_v<std::shared_ptr, value_type>) {
			static_assert(
				std::is_base_of_v<ruis::resource, typename value_type::element_type>,
				"shared_ptr must point to a ruis::resource"
			);
			if (desc.empty()) {
				return nullptr;
			}
			return loader.load<typename value_type::element_type>(desc.front().value.string);
		} else {
			return value_type::parse_value(desc);
		}
	}

public:
	style_value(
		const tml::forest& desc, //
		const ruis::resource_loader& loader
	) :
		value(load(desc, loader))
	{}

	const value_type& get_value() const noexcept
	{
		return this->value;
	}
};

template <typename value_type>
class styled
{
	friend class style;

	using style_value_ref_type = utki::shared_ref<const style_value<value_type>>;

	std::variant<
		style_value_ref_type, //
		value_type //
		>
		value;

	styled(style_value_ref_type sv) :
		value(std::move(sv))
	{}

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

	utki::shared_ref<ruis::style_sheet> cur_style_sheet;

	std::map<std::string, std::weak_ptr<style_value_base>, std::less<>> cache;

	std::shared_ptr<style_value_base> get_from_cache(std::string_view id);

	void store_to_cache(
		std::string_view id, //
		std::weak_ptr<style_value_base> v
	);

public:
	style(utki::shared_ref<ruis::resource_loader> loader);

	void set(utki::shared_ref<style_sheet> ss);

	template <typename value_type>
	styled<std::conditional_t<
		std::is_base_of_v<ruis::resource, value_type>, //
		std::shared_ptr<value_type>,
		value_type //
		>> //
	get(std::string_view id)
	{
		using actual_value_type = std::conditional_t<
			std::is_base_of_v<ruis::resource, value_type>, //
			std::shared_ptr<value_type>,
			value_type //
			>;

		if (auto svb = this->get_from_cache(id)) {
			auto sv = std::dynamic_pointer_cast<style_value<actual_value_type>>(svb);
			if (!sv) {
				throw std::invalid_argument(
					"style::get(id): requested value_type does not match the one stored in cache"
				);
			}
			return {utki::shared_ref<const style_value<actual_value_type>>(std::move(sv))};
		}

		const auto* desc = this->cur_style_sheet.get().get(id);
		if (!desc) {
			if constexpr (std::is_base_of_v<ruis::resource, value_type>) {
				return std::shared_ptr<value_type>();
			} else {
				return value_type::default_value();
			}
		}

		auto ret = utki::make_shared<style_value<actual_value_type>>(
			*desc, //
			this->loader.get()
		);
		this->store_to_cache(
			id, //
			ret.to_shared_ptr()
		);
		return {ret};
	}
};

} // namespace ruis
