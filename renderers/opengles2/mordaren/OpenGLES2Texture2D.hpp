#pragma once

#include <morda/render/Texture2D.hpp>

#include <GLES2/gl2.h>

namespace mordaren{

struct OpenGLES2Texture2D : public morda::Texture2D{
	GLuint tex;
	
	OpenGLES2Texture2D(kolme::Vec2f dim);
	
	~OpenGLES2Texture2D()noexcept;
	
	void bind(unsigned unitNum)const;
};


}
