#pragma once

#include "../core/Widget.hpp"


namespace morda{

/**
 * @brief Undocumented.
 */
class BlurGlass : virtual public Widget{
public:
	BlurGlass(const stob::Node* chain = nullptr);
	
	BlurGlass(const BlurGlass&) = delete;
	BlurGlass& operator=(const BlurGlass&) = delete;
	
	
	void render(const morda::Matr4r& matrix) const override;
	
private:

};

}
