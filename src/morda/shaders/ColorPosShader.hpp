#pragma once

#include "ColorShader.hpp"
#include "PosShader.hpp"



namespace morda{



/**
 * @brief Shader with position attribute and color uniform.
 * This shader assigns same color to all vertices.
 */
class ColorPosShader :
		public ColorShader,
		public PosShader
{
public:
	ColorPosShader();
	
	ColorPosShader(const ColorPosShader&) = delete;
	ColorPosShader& operator=(const ColorPosShader&) = delete;
};



}//~namespace
