#pragma once

#include <utki/shared_ref.hpp>
#include <tml/tree.hpp>

namespace ruis {

class style_value_base
{
	utki::shared_ref<style_sheet> owner_sheet;
public:
	virtual ~style_value_base() = default;

	virtual void reload(const tml::forest& desc) = 0;
};

class style_sheet
{
	std::shared_ptr<style_value_base> get(std::string_view style_name);

	// TODO: weak_ptr cache and reload cached values

public:
	template <typename value_type>
	utki::shared_ref<value_type> get(
		std::string_view style_name, //
		const value_type& default_value
	)
	{
		// TODO:
	}
};

template <typename value_type>
class styled
{
	std::variant<
	utki::shared_ref<style_value>,
	value_type> value;

public:
	styled(value_type value) :
		value(std::move(value))
	{}

	const value_type& get()const noexcept
	{
		// TODO:
	}
};

} // namespace ruis