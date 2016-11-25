#pragma once

#include "../../../src/morda/render/Texture2D.hpp"

#include <GL/glew.h>


struct OpenGL2Texture2D : public morda::Texture2D{
	GLuint tex;
	
	OpenGL2Texture2D(kolme::Vec2f dim);
	
	~OpenGL2Texture2D()noexcept;
	
	void bind(unsigned unitNum)const;
};
