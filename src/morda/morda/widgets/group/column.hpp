#pragma once

#include "linear_container.hpp"

namespace morda{

/**
 * @brief Vertical container widget.
 * column is a vertical variant of linear container. From GUI scripts it can be instantiated as "column".
 */
class column : public linear_container{
public:
	column(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			widget(std::move(c), desc),
			linear_container(nullptr, desc, true)
	{}

	column(const column&) = delete;
	column& operator=(const column&) = delete;
};

}
