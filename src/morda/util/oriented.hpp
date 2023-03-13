#pragma once

namespace morda{

class oriented{
    bool is_vertical_v;
public:
    unsigned get_long_index()const noexcept{
		return this->is_vertical_v ? 1 : 0;
	}

	unsigned get_trans_index()const noexcept{
		return this->is_vertical_v ? 0 : 1;
	}

	oriented(bool vertical) :
        is_vertical_v(vertical)
    {}

	bool is_vertical()const noexcept{
		return this->is_vertical_v;
	}
};

}
