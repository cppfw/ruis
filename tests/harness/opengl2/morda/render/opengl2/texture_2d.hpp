#pragma once

#include <morda/render/texture_2d.hpp>

#include <GL/glew.h>

namespace morda{ namespace render_opengl2{

struct texture_2d : public morda::texture_2d{
	GLuint tex;
	
	texture_2d(r4::vec2f dims);
	
	~texture_2d()noexcept;
	
	void bind(unsigned unitNum)const;
};

}}
