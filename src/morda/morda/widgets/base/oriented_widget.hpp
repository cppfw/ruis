#pragma once

#include "../widget.hpp"

namespace morda{

class oriented_widget : virtual public widget{
	bool is_vertical_v;

protected:
	unsigned get_long_index()const noexcept{
		return this->is_vertical_v ? 1 : 0;
	}

	unsigned get_trans_index()const noexcept{
		return this->is_vertical_v ? 0 : 1;
	}

	oriented_widget(std::shared_ptr<morda::context> c, const puu::forest& desc, bool vertical);

public:
	oriented_widget(const oriented_widget&) = delete;
	oriented_widget& operator=(const oriented_widget&) = delete;

	bool is_vertical()const noexcept{
		return this->is_vertical_v;
	}

	void set_vertical(bool vertical);

private:

};

}
