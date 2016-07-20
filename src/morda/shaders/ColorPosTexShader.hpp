#pragma once

#include "PosTexShader.hpp"
#include "ColorShader.hpp"


namespace morda{



/**
 * @brief Texturing shader with color uniform and position attribute.
 * This shader modulates the texture value with given color.
 */
class ColorPosTexShader :
		public PosTexShader,
		public ColorShader
{
public:
	ColorPosTexShader();
	
	ColorPosTexShader(const ColorPosTexShader&) = delete;
	ColorPosTexShader& operator=(const ColorPosTexShader&) = delete;
	
private:

};


}
