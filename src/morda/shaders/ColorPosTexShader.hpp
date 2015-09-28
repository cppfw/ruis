/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "PosTexShader.hpp"
#include "ColorShader.hpp"


namespace morda{



class ColorPosTexShader : public PosTexShader, public ColorShader{
public:
	ColorPosTexShader();
	
	ColorPosTexShader(const ColorPosTexShader&) = delete;
	ColorPosTexShader& operator=(const ColorPosTexShader&) = delete;
	
private:

};


}
