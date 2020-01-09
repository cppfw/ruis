#pragma once

#include "LinearContainer.hpp"

namespace morda{

/**
 * @brief Vertical container widget.
 * Column is a vertical variant of linear container. From GUI scripts it can be instantiated as "Column".
 */
class Column : public LinearContainer{
public:
	Column(const puu::forest& desc) :
			widget(desc),
			LinearContainer(desc, true)
	{}
	Column(const stob::Node* chain) : Column(stob_to_puu(chain)){}

	Column(const Column&) = delete;
	Column& operator=(const Column&) = delete;
};

}
