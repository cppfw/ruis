#pragma once

#include "LinearContainer.hpp"

namespace morda{

/**
 * @brief Vertical container widget.
 * column is a vertical variant of linear container. From GUI scripts it can be instantiated as "column".
 */
class column : public LinearContainer{
public:
	column(std::shared_ptr<morda::context> c, const puu::forest& desc) :
			widget(std::move(c), desc),
			LinearContainer(nullptr, desc, true)
	{}

	column(const column&) = delete;
	column& operator=(const column&) = delete;
};

}
