/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <utki/Singleton.hpp>

#include "ColorShader.hpp"
#include "PosShader.hpp"



namespace morda{



class ColorPosShader : public ColorShader, public PosShader{
public:
	ColorPosShader();
	
	ColorPosShader(const ColorPosShader&) = delete;
	ColorPosShader& operator=(const ColorPosShader&) = delete;
};



}//~namespace
