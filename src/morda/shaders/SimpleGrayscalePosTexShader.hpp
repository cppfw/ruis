/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "PosTexShader.hpp"


namespace morda{

class SimpleGrayscalePosTexShader : public PosTexShader{
public:
	SimpleGrayscalePosTexShader();
	
	SimpleGrayscalePosTexShader(const SimpleGrayscalePosTexShader&) = delete;
	SimpleGrayscalePosTexShader& operator=(const SimpleGrayscalePosTexShader&) = delete;
	
private:

};

}
