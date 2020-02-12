#pragma once

#include "LinearContainer.hpp"

namespace morda{

/**
 * @brief Horizontal container widget.
 * Row is a horizontal variant of linear container. From GUI scripts it can be instantiated as "Row".
 */
class Row : public LinearContainer{
public:
	Row(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc),
			LinearContainer(this->context, desc, false)
	{}

	Row(const Row&) = delete;
	Row& operator=(const Row&) = delete;
};

}
