#pragma once

#include "LinearContainer.hpp"

namespace morda{

/**
 * @brief Vertical container widget.
 * Column is a vertical variant of linear container. From GUI scripts it can be instantiated as "Column".
 */
class Column : public LinearContainer{
public:
	Column(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc),
			LinearContainer(nullptr, desc, true)
	{}

	Column(const Column&) = delete;
	Column& operator=(const Column&) = delete;
};

}
