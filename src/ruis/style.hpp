#pragma once

namespace ruis {

class style_sheet
{
public:
};

template <typename value_type>
class style_value
{
public:
	utki::shared_ref<style_sheet> ss;

	value_type value;
};

template <typename value_type>
class styled
{
	std::shared_ptr<style_value> value;
	value_type cur_value;

public:
	styled(value_type value) :
		cur_value(std::move(value))
	{}

	const value_type& get()
	{
		this->cur_value;
	}

	void reload()
	{
		if(!this->value){
			return;
		}

		// TODO:
	}
};

} // namespace ruis