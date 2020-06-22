#pragma once

#include "image.hpp"

#include "../../res/image.hpp"

#include "../../updateable.hpp"

namespace morda{

/**
 * @brief Busy spinner.
 */
class busy :
		public image,
		public updateable
{
	real angle = 0;
public:
	busy(std::shared_ptr<morda::context> c, const puu::forest& desc);

	void set_active(bool active);

	void render(const matrix4& matrix)const override;
private:
	void update(uint32_t dt_ms)override;
};

}
