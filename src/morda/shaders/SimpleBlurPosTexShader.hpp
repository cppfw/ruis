#pragma once

#include "PosTexShader.hpp"


namespace morda{

/**
 * @brief Undocumented.
 */
class SimpleBlurPosTexShader : public PosTexShader{
	Render::InputID texStepUniform;
	
public:
	SimpleBlurPosTexShader();
	
	SimpleBlurPosTexShader(const SimpleBlurPosTexShader&) = delete;
	SimpleBlurPosTexShader& operator=(const SimpleBlurPosTexShader&) = delete;
	
	void setTextureStep(kolme::Vec2f step){
		this->setUniform2f(this->texStepUniform, step);
	}
	
private:

};

}
