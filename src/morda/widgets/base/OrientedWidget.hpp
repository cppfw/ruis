#pragma once

#include "../widget.hpp"

namespace morda{

class OrientedWidget : virtual public Widget{
	bool isVertical_v;

protected:
	unsigned getLongIndex()const noexcept{
		return this->isVertical_v ? 1 : 0;
	}

	unsigned getTransIndex()const noexcept{
		return this->isVertical_v ? 0 : 1;
	}

	OrientedWidget(const puu::forest& desc, bool vertical);

	OrientedWidget(const stob::Node* chain, bool vertical) :
			OrientedWidget(stob_to_puu(chain), vertical)
	{}

public:
	OrientedWidget(const OrientedWidget&) = delete;
	OrientedWidget& operator=(const OrientedWidget&) = delete;

	bool isVertical()const noexcept{
		return this->isVertical_v;
	}

	void setVertical(bool vertical);

private:

};

}
