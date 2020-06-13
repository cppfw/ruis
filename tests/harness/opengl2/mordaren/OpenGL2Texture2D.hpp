#pragma once

#include <morda/render/texture_2d.hpp>

#include <GL/glew.h>

namespace mordaren{

struct OpenGL2Texture2D : public morda::texture_2d{
	GLuint tex;
	
	OpenGL2Texture2D(r4::vec2f dims);
	
	~OpenGL2Texture2D()noexcept;
	
	void bind(unsigned unitNum)const;
};


}
