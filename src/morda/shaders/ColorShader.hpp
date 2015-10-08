/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once


#include <utki/debug.hpp>

#include <kolme/Vector4.hpp>

#include "../render/Shader.hpp"



namespace morda{



/**
 * @brief Abstract shader with uniform coloring feature.
 * The uniform for color should be 'vec4' and named 'uniformColor'.
 */
class ColorShader : virtual public Shader{

	//no copying
	ColorShader(const ColorShader&);
	ColorShader& operator=(const ColorShader&);

	Render::InputID colorUniform;

protected:
	ColorShader() :
			colorUniform(this->getUniform("uniformColor"))
	{}

public:
	void SetColor(std::uint32_t color){
		this->SetColor(
				float(color & 0xff) / float(0xff),
				float((color >> 8) & 0xff) / float(0xff),
				float((color >> 16) & 0xff) / float(0xff),
				float((color >> 24) & 0xff) / float(0xff)
			);
	}
	
	void SetColor(kolme::Vec3f color){
		this->setUniform4f(this->colorUniform, color.x, color.y, color.z, 1.0f);
    }

	void SetColor(kolme::Vec3f color, float alpha){
		this->setUniform4f(this->colorUniform, color.x, color.y, color.z, alpha);
    }
	
	void SetColor(float r, float g, float b, float a){
		this->setUniform4f(this->colorUniform, r, g, b, a);
	}

	void SetColor(const kolme::Vec4f& color){
		this->setUniform4f(this->colorUniform, utki::wrapBuf(&color, 1));
    }
};



}//~namespace
