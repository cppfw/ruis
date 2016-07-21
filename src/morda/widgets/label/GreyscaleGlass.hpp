#pragma once

#include "../core/Widget.hpp"

namespace morda{

/**
 * @brief Undocumented.
 */
class GreyscaleGlass : virtual public Widget{
public:
	GreyscaleGlass(const stob::Node* chain = nullptr);
	
	GreyscaleGlass(const GreyscaleGlass&) = delete;
	GreyscaleGlass& operator=(const GreyscaleGlass&) = delete;
	
private:

	void render(const morda::Matr4r& matrix) const override;
};

}
