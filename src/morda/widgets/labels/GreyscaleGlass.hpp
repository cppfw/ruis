/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "../Widget.hpp"

namespace morda{

class GreyscaleGlass : virtual public Widget{
public:
	GreyscaleGlass(const stob::Node* chain = nullptr);
	
	GreyscaleGlass(const GreyscaleGlass&) = delete;
	GreyscaleGlass& operator=(const GreyscaleGlass&) = delete;
	
private:

	void render(const morda::Matr4r& matrix) const override;
};

}
