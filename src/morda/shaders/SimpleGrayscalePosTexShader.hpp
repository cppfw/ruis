#pragma once

#include "PosTexShader.hpp"


namespace morda{

/**
 * @brief Undocumented.
 */
class SimpleGrayscalePosTexShader : public PosTexShader{
public:
	SimpleGrayscalePosTexShader();
	
	SimpleGrayscalePosTexShader(const SimpleGrayscalePosTexShader&) = delete;
	SimpleGrayscalePosTexShader& operator=(const SimpleGrayscalePosTexShader&) = delete;
	
private:

};

}
