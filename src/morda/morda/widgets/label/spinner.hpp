#pragma once

#include "image.hpp"

#include "../../res/image.hpp"

#include "../../updateable.hpp"

namespace morda{

/**
 * @brief Spinning image label.
 * @param active - whether the spinner is initially active or not, can be true/false.
 */
class spinner :
		public image,
		public updateable
{
	real angle = 0;
public:
	spinner(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	void set_active(bool active);

	void render(const matrix4& matrix)const override;
private:
	void update(uint32_t dt_ms)override;
};

}
