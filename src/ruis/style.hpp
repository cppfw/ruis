#pragma once

#include <tml/tree.hpp>
#include <utki/shared_ref.hpp>

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
public:
};

class style
{
	std::shared_ptr<style_value_base> get(std::string_view style_name);

	// TODO: weak_ptr cache and reload cached values

public:
	void set(style_sheet ss);

	template <typename value_type>
	styled<value_type> get(std::string_view style_name)
	{
		// TODO:
	}

	template <typename value_type>
	styled<value_type> get(
		std::string_view style_name, //
		const value_type& default_value
	)
	{
		// TODO:
	}
};

} // namespace ruis
