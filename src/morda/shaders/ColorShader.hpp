#pragma once


#include <utki/debug.hpp>

#include <kolme/Vector4.hpp>

#include "../render/Shader.hpp"



namespace morda{



/**
 * @brief Abstract shader with uniform coloring feature.
 * The uniform for color should be 'vec4' and named 'uniformColor' in GLSL.
 */
class ColorShader : virtual public Shader{

	Render::InputID colorUniform;

protected:
	ColorShader() :
			Shader(nullptr, nullptr),
			colorUniform(this->getUniform("uniformColor"))
	{}

public:
	
	ColorShader(const ColorShader&) = delete;
	ColorShader& operator=(const ColorShader&) = delete;
	
	/**
	 * @brief Set value of color uniform.
	 * @param color - color value to set.
	 */
	void setColor(std::uint32_t color){
		this->setColor(
				float(color & 0xff) / float(0xff),
				float((color >> 8) & 0xff) / float(0xff),
				float((color >> 16) & 0xff) / float(0xff),
				float((color >> 24) & 0xff) / float(0xff)
			);
	}
	
	/**
	 * @brief Set value of color uniform.
	 * @param color - color value to set.
	 */
	void setColor(kolme::Vec3f color){
		this->setUniform4f(this->colorUniform, color.x, color.y, color.z, 1.0f);
    }

	/**
	 * @brief Set value of color uniform.
	 * @param color - RGB components of color value to set.
	 * @param alpha - alpha component of color value to set.
	 */
	void setColor(kolme::Vec3f color, float alpha){
		this->setUniform4f(this->colorUniform, color.x, color.y, color.z, alpha);
    }
	
	/**
	 * @brief Set value of color uniform.
	 * @param r - red component of color value to set.
	 * @param g - green component of color value to set.
	 * @param b - blue component of color value to set.
	 * @param a - alpha component of color value to set.
	 */
	void setColor(float r, float g, float b, float a){
		this->setUniform4f(this->colorUniform, r, g, b, a);
	}

	/**
	 * @brief Set value of color uniform.
	 * @param color - color value to set.
	 */
	void setColor(const kolme::Vec4f& color){
		this->setUniform4f(this->colorUniform, utki::wrapBuf(&color, 1));
    }
};



}
