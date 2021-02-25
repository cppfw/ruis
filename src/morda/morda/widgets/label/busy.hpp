#pragma once

#include "spinner.hpp"

namespace morda{

/**
 * @brief Busy spinner.
 * @param active - whether the busy indicator is initially active or not, can be true/false.
 */
class busy : public spinner{
public:
	busy(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	void set_active(bool active);
};

}
