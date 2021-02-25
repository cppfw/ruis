#pragma once

#include "linear_container.hpp"

namespace morda{

/**
 * @brief Horizontal container widget.
 * Row is a horizontal variant of linear container. From GUI scripts it can be instantiated as "row".
 */
class row : public linear_container{
public:
	row(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
			widget(std::move(c), desc),
			linear_container(this->context, desc, false)
	{}

	row(const row&) = delete;
	row& operator=(const row&) = delete;
};

}
