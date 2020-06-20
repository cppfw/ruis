#pragma once

#include <morda/render/Texture2D.hpp>

#include <GL/glew.h>


struct texture_2d : public morda::Texture2D{
	GLuint tex;
	
	texture_2d(kolme::Vec2f dim);
	
	~texture_2d()noexcept;
	
	void bind(unsigned unitNum)const;
};
