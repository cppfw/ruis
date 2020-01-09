#pragma once

#include "LinearContainer.hpp"

namespace morda{

/**
 * @brief Horizontal container widget.
 * Row is a horizontal variant of linear container. From GUI scripts it can be instantiated as "Row".
 */
class Row : public LinearContainer{
public:
	Row(const puu::forest& desc) :
			widget(desc),
			LinearContainer(desc, false)
	{}

	Row(const Row&) = delete;
	Row& operator=(const Row&) = delete;
};

}
